# Orca-MPW GitHub package builds

The tested multi-printer project-saving source is preserved on
`release/mpw-tested-2026-11-13` at commit `a90ec844b5bf`.
Packaging workflow fixes are developed on `build/mpw-packages`.

## Build installers on GitHub

1. Open **Actions → Build all → Run workflow**.
2. Select branch `build/mpw-packages` (or a release branch containing these fixes).
3. Set **Only build dependencies** to **false** and **Select a platform** to **all**.
4. Wait for the platform jobs and the **Package MPW for Arch and Debian** job.
5. Download the platform artifacts from the completed Actions run.

A manual Build Linux workflow can produce the native Linux packages without
building Windows or macOS. Select the same branch and leave
**Build dependencies only** unchecked.

| Target | Actions artifact | Install |
| --- | --- | --- |
| Arch Linux x86-64 | `Orca-MPW-Arch-x86_64` | `sudo pacman -U ./orca-mpw-*-x86_64.pkg.tar.zst` |
| Debian amd64 | `Orca-MPW-Debian-amd64` | `sudo apt install ./orca-mpw_*_amd64.deb` |
| Linux portable | `OrcaSlicer_Linux_ubuntu_*` | Extract the artifact ZIP and run the AppImage |
| Windows x64/ARM64 | `OrcaSlicer_Windows_V*` | Run the installer for the matching architecture |
| macOS Intel/Apple silicon | `OrcaSlicer_Mac_universal_*` | Open the DMG and drag the app to Applications |

Artifacts in Actions are **build outputs**, not automatically published releases.
To publish, create a **draft prerelease** with a dedicated tag, then run
**Actions → Publish to draft release** using the completed Build all run ID
and that tag. The publish workflow includes both native Linux packages.

## Safety and validation

The Linux native packages install under `/opt/orca-mpw` and provide the
`orca-mpw` executable and a separate **OrcaSlicer MPW** desktop launcher.
They do not remove your existing `orca-slicer-nightly-bin` package.

Windows/macOS builds currently retain upstream app naming and are not signed
with upstream's credentials. Test these installers on a clean OS account or VM.

**Do not label an artifact stable until its CI build, package creation,
installation, first launch, and 3MF save/reopen tests have passed.**
