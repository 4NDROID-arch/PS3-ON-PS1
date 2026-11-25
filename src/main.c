#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <psxetc.h>
#include <psxgpu.h>
#include <psxpad.h>

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
static char pad_buffer[2][34];

static int db = 0;
static int page = PAGE_BOOT;
static int category = 5;
static int selection = 0;
static int previous_category = 5;
static int debounce = 0;

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

static void init_system(void) {
    ResetGraph(0);

    SetDefDispEnv(&disp[0], 0, 0, SCREEN_W, SCREEN_H);
    SetDefDispEnv(&disp[1], 0, SCREEN_H, SCREEN_W, SCREEN_H);

    SetDefDrawEnv(&draw[0], 0, SCREEN_H, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_W, SCREEN_H);

    setRGB0(&draw[0], 5, 14, 38);
    setRGB0(&draw[1], 5, 14, 38);
    draw[0].isbg = 1;
    draw[1].isbg = 1;

    FntLoad(960, 0);
    FntOpen(8, 8, 304, 220, 0, 1024);

    EnterCriticalSection();
    InitPAD(pad_buffer[0], 34, pad_buffer[1], 34);
    StartPAD();
    ChangeClearPAD(0);
    ExitCriticalSection();

    PutDispEnv(&disp[0]);
    PutDrawEnv(&draw[0]);
    SetDispMask(1);
}

static PADTYPE *read_pad(void) {
    PADTYPE *pad = (PADTYPE *)pad_buffer[0];

    if (pad->stat != 0) {
        return 0;
    }

    /* 0x4, 0x5 and 0x7 are digital/analog controller types. */
    if (pad->type != 0x4 && pad->type != 0x5 && pad->type != 0x7) {
        return 0;
    }

    return pad;
}

static int pressed(PADTYPE *pad, uint16_t button) {
    /* PS1 controller button bits are active-low. */
    return pad && !(pad->btn & button);
}

static void draw_boot(void) {
    FntPrint(-1, "\n\n\n");
    FntPrint(-1, "              PLAYSTATION\n\n");
    FntPrint(-1, "        SYSTEM EXPERIENCE 3.55\n\n\n");
    FntPrint(-1, "        Press START or CROSS\n\n");
    FntPrint(-1, "       Local PS1 interface demo");
}

static void draw_user(void) {
    FntPrint(-1, "USER SELECTION\n");
    FntPrint(-1, "----------------------------------------\n\n");
    FntPrint(-1, "                 > Guest\n\n");
    FntPrint(-1, "                   Create local user\n\n");
    FntPrint(-1, "                   Switch user\n\n\n");
    FntPrint(-1, "          CROSS: Select  CIRCLE: Back\n");
}

static void draw_xmb(void) {
    int i;

    FntPrint(-1, "PS3 SYSTEM 3.55                    USER: Guest\n");
    FntPrint(-1, "-----------------------------------------------\n\n");

    for (i = 0; i < CATEGORY_COUNT; i++) {
        if (i == category) {
            FntPrint(-1, " [%s] ", categories[i]);
        } else {
            FntPrint(-1, "  %s  ", categories[i]);
        }
    }

    FntPrint(-1, "\n\n\n");

    for (i = 0; i < ITEM_COUNT; i++) {
        if (i == selection) {
            FntPrint(-1, "          > %s\n\n", items[category][i]);
        } else {
            FntPrint(-1, "            %s\n\n", items[category][i]);
        }
    }

    FntPrint(-1, "\nD-PAD: Navigate  CROSS: Open  CIRCLE: Back\n");
    FntPrint(-1, "             XMB INTERFACE SIMULATION");
}

static void draw_detail(void) {
    FntPrint(-1, "\n\n");
    FntPrint(-1, "             %s\n\n", items[previous_category][selection]);
    FntPrint(-1, "-----------------------------------------------\n\n");
    FntPrint(-1, "       This is a local interface simulation.\n\n");
    FntPrint(-1, "       Online services and a real browser\n");
    FntPrint(-1, "       are not available in this PS1 demo.\n\n\n");
    FntPrint(-1, "       Press CIRCLE to return to XMB.");
}

static void update_input(void) {
    PADTYPE *pad = read_pad();

    if (debounce > 0) {
        debounce--;
        return;
    }

    if (!pad) {
        return;
    }

    if (page == PAGE_BOOT) {
        if (pressed(pad, PAD_START) || pressed(pad, PAD_CROSS)) {
            page = PAGE_USER;
            debounce = 12;
        }
        return;
    }

    if (page == PAGE_USER) {
        if (pressed(pad, PAD_CROSS) || pressed(pad, PAD_START)) {
            page = PAGE_XMB;
            debounce = 12;
        } else if (pressed(pad, PAD_CIRCLE)) {
            page = PAGE_BOOT;
            debounce = 12;
        }
        return;
    }

    if (page == PAGE_DETAIL) {
        if (pressed(pad, PAD_CIRCLE)) {
            page = PAGE_XMB;
            debounce = 12;
        }
        return;
    }

    if (page == PAGE_XMB) {
        if (pressed(pad, PAD_LEFT)) {
            category = (category + CATEGORY_COUNT - 1) % CATEGORY_COUNT;
            selection = 0;
            debounce = 8;
        } else if (pressed(pad, PAD_RIGHT)) {
            category = (category + 1) % CATEGORY_COUNT;
            selection = 0;
            debounce = 8;
        } else if (pressed(pad, PAD_UP)) {
            selection = (selection + ITEM_COUNT - 1) % ITEM_COUNT;
            debounce = 8;
        } else if (pressed(pad, PAD_DOWN)) {
            selection = (selection + 1) % ITEM_COUNT;
            debounce = 8;
        } else if (pressed(pad, PAD_CROSS)) {
            previous_category = category;
            if (selection == ITEM_COUNT - 1) {
                page = PAGE_USER;
            } else {
                page = PAGE_DETAIL;
            }
            debounce = 12;
        } else if (pressed(pad, PAD_CIRCLE)) {
            page = PAGE_USER;
            debounce = 12;
        }
    }
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

int main(void) {
    init_system();

    while (1) {
        update_input();

        switch (page) {
            case PAGE_BOOT:
                draw_boot();
                break;
            case PAGE_USER:
                draw_user();
                break;
            case PAGE_XMB:
                draw_xmb();
                break;
            case PAGE_DETAIL:
                draw_detail();
                break;
            default:
                page = PAGE_BOOT;
                break;
        }

        draw_frame();
    }

    return 0;
}
