# Orca-MPW preview downloads

In this fork, open **Actions → Build all → Run workflow**, select
`review/orca-mpw-v2.5.0`, and leave **Only build dependencies** unchecked.
Open the finished run and download the artifact for your OS. Extract the
artifact ZIP before running the commands below.

| Platform | Artifact | Install |
| --- | --- | --- |
| Arch Linux x86-64 | `Orca-MPW-Arch-x86_64` | `sudo pacman -U ./orca-mpw-*-x86_64.pkg.tar.zst` |
| Debian 13 amd64 | `Orca-MPW-Debian-amd64` | `sudo apt install ./orca-mpw_*_amd64.deb` |
| Windows x64 or ARM64 | `OrcaSlicer_Windows_V*` | Run the installer EXE matching your CPU. |
| macOS Intel or Apple silicon | `OrcaSlicer_Mac_universal_*` | Open the DMG and drag the app to Applications. |

The Linux packages use the application payload from the same build's AppImage
and add a desktop launcher named **OrcaSlicer MPW**. Debian 13 is the target for
the DEB because its runtime libraries meet the Ubuntu 24.04 build baseline.
The same run also provides portable Linux AppImages.

Fork builds use upstream application naming on Windows and macOS and do not
have upstream signing credentials. Use a fresh OS user or VM for the first
install test so it starts with a clean Orca configuration.

After testing, attach the four installer formats from the same run to a draft
prerelease. See [the regression checklist](ORCA-MPW-REVIEW.md) for the current
validation status.
