# PS3 SYSTEM 3.55 – PS1 Edition

Starter homebrew project: a PS1 disc that recreates the *basic visual flow* of a PS3 3.55-style XMB:
- boot / welcome screen
- local user selection
- horizontal XMB categories
- vertical menu navigation
- local placeholder pages for Games, Network/Browser, Settings and Users

This is a local interface simulation, not Sony firmware. It does not run PS3 software,
connect to PSN, browse the real web, or use Sony's proprietary assets. All branding,
artwork, icons and audio should be original or properly licensed.

## Controls
- D-pad: move through categories and items
- Cross: open/select
- Circle: back
- Start: continue from boot screen

## Build
The project uses PSn00bSDK + CMake. PSn00bSDK requires CMake 3.21+ and a
`mipsel-none-elf` toolchain. Install a prebuilt PSn00bSDK release and set:

```sh
export PATH="/path/to/PSn00bSDK/bin:$PATH"
export PSN00BSDK_LIBS="/path/to/PSn00bSDK/lib/libpsn00b"
cmake --preset default
cmake --build build
```

Expected disc outputs are `build/ps3_system_355.bin` and `build/ps3_system_355.cue`.
The BIN/CUE pair is the normal PS1 CD image representation. If an `.iso` file is
specifically required, convert the data track with a compatible disc-image tool;
do not rename `.bin` to `.iso`.

## GitHub Actions
Push this folder to a GitHub repository. The workflow installs CMake/Ninja and
fetches the latest Linux PSn00bSDK release asset containing the bundled Linux
toolchain, then builds and uploads the BIN/CUE artifacts.

If the SDK release asset naming changes, update the asset selection step in
`.github/workflows/build.yml` to match the official release page:
https://github.com/Lameguy64/PSn00bSDK/releases

## Project status
This is the first bootable UI prototype. The current screen is intentionally
text-first; next steps are custom TIM graphics, animated wave background,
profile persistence, and richer XMB transitions.
