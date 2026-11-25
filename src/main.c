#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_W 320
#define SCREEN_H 240
#define CATEGORY_COUNT 6
#define ITEM_COUNT 5

typedef enum {
    PAGE_BOOT = 0,
    PAGE_USER,
    PAGE_XMB,
    PAGE_DETAIL
} Page;

static DISPENV disp[2];
static DRAWENV draw[2];
static int db = 0;

static const char *categories[CATEGORY_COUNT] = {
    "USERS", "SETTINGS", "PHOTOS", "MUSIC", "VIDEO", "GAMES"
};

static const char *items[CATEGORY_COUNT][ITEM_COUNT] = {
    {"Guest", "Create local user", "Switch user", "User information", "Back"},
    {"System Settings", "Theme / Background", "Date and Time", "Display Settings", "Back"},
    {"Photo Gallery", "View sample album", "Slideshow (demo)", "Import (unavailable)", "Back"},
    {"Music Player", "Sample playlist", "Now Playing (demo)", "Audio Settings", "Back"},
    {"Video Player", "Sample videos", "Playback Settings", "Video Information", "Back"},
    {"Game Library", "Start Demo Tile", "Memory Card (PS1)", "Disc Information", "Back"}
};

static int category = 5;
static int selection = 0;
static int previous_category = 5;
static Page page = PAGE_BOOT;
static int user_selected = 0;
static int debounce = 0;
static int font_id = -1;

static void init_video(void) {
    ResetGraph(0);
    PadInit(0);

    SetDefDispEnv(&disp[0], 0, 0, SCREEN_W, SCREEN_H);
    SetDefDispEnv(&disp[1], 0, SCREEN_H, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&draw[0], 0, SCREEN_H, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_W, SCREEN_H);

    setRGB0(&draw[0], 4, 10, 28);
    setRGB0(&draw[1], 4, 10, 28);
    draw[0].isbg = 1;
    draw[1].isbg = 1;

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    SetDispMask(1);

    FntLoad(960, 0);
    font_id = FntOpen(8, 8, 304, 220, 0, 1024);
}

static void draw_frame(void) {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    SetDispMask(1);
    FntFlush(-1);
    db ^= 1;
}

static void draw_boot(void) {
    FntPrint(font_id, "\n\n\n");
    FntPrint(font_id, "                 PLAYSTATION\n\n");
    FntPrint(font_id, "          SYSTEM EXPERIENCE 3.55\n\n\n");
    FntPrint(font_id, "          Press START to continue\n\n");
    FntPrint(font_id, "       PS1 HOMEbrew interface simulation");
}

static void draw_user(void) {
    FntPrint(font_id, "USER SELECTION\n");
    FntPrint(font_id, "------------------------------------------------\n\n");
    FntPrint(font_id, "                 > Guest\n\n");
    FntPrint(font_id, "                   Create local user\n\n");
    FntPrint(font_id, "                   Switch user\n\n\n");
    FntPrint(font_id, "          CROSS: Select       CIRCLE: Back\n");
}

static void draw_xmb(void) {
    int i;
    FntPrint(font_id, "PS3 SYSTEM 3.55                         09:41\n");
    FntPrint(font_id, "------------------------------------------------\n\n");

    for (i = 0; i < CATEGORY_COUNT; i++) {
        if (i == category) FntPrint(font_id, "  [%s]  ", categories[i]);
        else FntPrint(font_id, "   %s   ", categories[i]);
    }

    FntPrint(font_id, "\n\n\n");
    for (i = 0; i < ITEM_COUNT; i++) {
        if (i == selection) FntPrint(font_id, "          > %s\n\n", items[category][i]);
        else FntPrint(font_id, "            %s\n\n", items[category][i]);
    }

    FntPrint(font_id, "\n\n");
    FntPrint(font_id, "D-PAD: Navigate   CROSS: Open   CIRCLE: Back\n");
    FntPrint(font_id, "Local user: Guest                 XMB prototype");
}

static void draw_detail(void) {
    FntPrint(font_id, "\n\n");
    FntPrint(font_id, "                  %s\n\n", items[previous_category][selection]);
    FntPrint(font_id, "------------------------------------------------\n\n");
    FntPrint(font_id, "       This page is a local interface demo.\n\n");
    FntPrint(font_id, "       Online services and real browser are\n");
    FntPrint(font_id, "       not available in this PS1 simulation.\n\n\n");
    FntPrint(font_id, "       Press CIRCLE to return to XMB.");
}

static void update_input(void) {
    unsigned long pad = PadRead(0);

    if (debounce > 0) {
        debounce--;
        return;
    }

    if (page == PAGE_BOOT) {
        if (pad & (PADstart | PADcross)) {
            page = PAGE_USER;
            debounce = 15;
        }
        return;
    }

    if (page == PAGE_USER) {
        if (pad & (PADcross | PADstart)) {
            user_selected = 1;
            page = PAGE_XMB;
            debounce = 15;
        } else if (pad & PADcircle) {
            page = PAGE_BOOT;
            debounce = 15;
        }
        return;
    }

    if (page == PAGE_DETAIL) {
        if (pad & PADcircle) {
            page = PAGE_XMB;
            debounce = 12;
        }
        return;
    }

    if (page == PAGE_XMB) {
        if (pad & PADleft) {
            category = (category + CATEGORY_COUNT - 1) % CATEGORY_COUNT;
            selection = 0;
            debounce = 10;
        } else if (pad & PADright) {
            category = (category + 1) % CATEGORY_COUNT;
            selection = 0;
            debounce = 10;
        } else if (pad & PADup) {
            selection = (selection + ITEM_COUNT - 1) % ITEM_COUNT;
            debounce = 10;
        } else if (pad & PADdown) {
            selection = (selection + 1) % ITEM_COUNT;
            debounce = 10;
        } else if (pad & PADcross) {
            previous_category = category;
            if (selection == ITEM_COUNT - 1) {
                page = PAGE_USER;
            } else {
                page = PAGE_DETAIL;
            }
            debounce = 15;
        } else if (pad & PADcircle) {
            page = PAGE_USER;
            debounce = 15;
        }
    }
}

int main(void) {
    init_video();

    while (1) {
        update_input();

        switch (page) {
            case PAGE_BOOT:   draw_boot(); break;
            case PAGE_USER:   draw_user(); break;
            case PAGE_XMB:    draw_xmb(); break;
            case PAGE_DETAIL: draw_detail(); break;
        }

        draw_frame();
    }

    return 0;
}
