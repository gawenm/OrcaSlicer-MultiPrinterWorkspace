# OrcaSlicer Multi-Printer Workspace (MPW)

An independent development fork of [OrcaSlicer](https://github.com/OrcaSlicer/OrcaSlicer) for managing multiple printer configurations in a single project. MPW is not an official OrcaSlicer release.

**Status: development preview.** Do not rely on an unverified build for unattended printing.

## What MPW adds

- Printer-specific rows and plate assignments within a single workspace.
- Per-plate printer, process, filament, bed and nozzle settings.
- Printer-aware device selection and print-host routing.
- Project persistence for printer assignments and slicing settings.
- Native OrcaSlicer controls and workflows wherever possible.

These feature areas remain under validation across printer combinations and plugins.

## Downloads

Check [MPW Releases](https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace/releases) for published packages. Development artifacts from [GitHub Actions](https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace/actions) may require sign-in and expire.

An earlier development build, `340dff40` (2.6.0-dev), has [CI artifacts](https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace/actions/runs/45100417050) for Windows x64 and Linux x86_64/ARM64. These are not the current branch's verified release packages.

Target platforms: Linux x86_64, Windows x64, and macOS (Intel/Apple Silicon). Additional Linux ARM64 packages may be offered when validated. Windows ARM64 is not a release target. Availability depends on build and package checks.

Upstream Microsoft Store, winget, Homebrew and Flathub packages install upstream OrcaSlicer, not MPW. Upstream code-signing guarantees do not apply to this fork.

## Getting started

1. Back up your OrcaSlicer configuration and project files before testing.
2. Install or extract an MPW package for your platform. On Linux, an AppImage may need executable permission.
3. Configure the intended printer presets and connections locally.
4. Assign printers to workspace rows and add plates to the appropriate row.
5. Check the printer, bed, nozzle, filament and process before slicing.
6. Check the physical device and destination before sending a job.
7. Save a separate project copy and verify assignments survive reopening.

Do not assume that MPW projects preserve multi-printer assignments when reopened and saved by upstream OrcaSlicer or older builds.

## Known limitations and testing

- Mixed-printer routing, stale device callbacks and UI responsiveness remain under investigation.
- Third-party printer agents may implement only a subset of native controls; telemetry does not guarantee command support.
- A missing or ambiguous device binding must not silently select a different printer.
- A successful compile does not establish correct device routing or safe generated G-code.

Automated device tests must use simulated devices or mocked transports. Release candidates require mixed-printer slicing, persistence, dispatch, single-printer regression and platform installation checks.

## Report a problem

Use [MPW Issues](https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace/issues). Include version and commit, operating system, printer models and agent types, reproduction steps, expected and actual behavior, and sanitized logs or a minimal project where possible. Remove access codes, credentials and private network information before posting.

## Upstream documentation and building

See the [upstream OrcaSlicer wiki](https://www.orcaslicer.com/wiki) and [build guide](https://www.orcaslicer.com/wiki/how_to_build) for inherited slicing features and build prerequisites. Fork-specific builds use this repository's scripts and [CI workflows](https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace/actions).

For upstream downloads, support, community and sponsor information, visit the [upstream repository](https://github.com/OrcaSlicer/OrcaSlicer).

## Some Background

Open-source slicing has always been built on a tradition of collaboration and attribution. [Slic3r](https://github.com/Slic3r/Slic3r), created by Alessandro Ranellucci and the RepRap community, laid the foundation. [PrusaSlicer](https://github.com/prusa3d/PrusaSlicer) by Prusa Research built on Slic3r and acknowledged that heritage. [Bambu Studio](https://github.com/bambulab/BambuStudio) in turn forked from PrusaSlicer, and [SuperSlicer](https://github.com/supermerill/SuperSlicer) by @supermerill extended PrusaSlicer with community-driven enhancements. Each project carried the work of its predecessors forward, crediting those who came before.

OrcaSlicer began in that same spirit, drawing from BambuStudio, PrusaSlicer, and ideas inspired by CuraSlicer and SuperSlicer. But it has since grown far beyond its origins. Through relentless innovation — introducing advanced calibration tools, precise wall and seam control, tree supports, adaptive slicing, and hundreds of other features — OrcaSlicer has become the most widely used and actively developed open-source slicer in the 3D printing community. Many of its innovations have been adopted by other slicers, making it a driving force for the entire industry.

The OrcaSlicer logo was designed by community member [Justin Levine](https://github.com/jal-co).


# License

- **OrcaSlicer** is licensed under the GNU Affero General Public License, version 3.
- OrcaSlicer includes a **pressure advance calibration pattern test** adapted from Andrew Ellis' generator, which is licensed under GNU General Public License, version 3. Ellis' generator is itself adapted from a generator developed by Sineos for Marlin, which is licensed under GNU General Public License, version 3.
- The **Bambu networking plugin** is based on non-free libraries from BambuLab. It is optional to the OrcaSlicer and provides extended functionalities for Bambulab printer users.

