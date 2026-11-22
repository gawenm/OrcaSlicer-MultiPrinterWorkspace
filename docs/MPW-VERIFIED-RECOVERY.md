# MPW verified-fix recovery candidate (2026-11-21)

**Status: recovery source candidate; NOT a validated installer or stable release.**

This branch starts from the earlier `27b854c6ae` per-plate networking and native filament integration instead of the unverified upstream-`main` preview, and reapplies the printer-restoration ordering fix from `a90ec844b5bf` (ported in `70cf377c1c00`). No original branches or installed packages are overwritten.

## Source fixes retained

| Area | Evidence | Scope and limitation |
|---|---|---|
| Printer rows / selected-plate state | Present in MPW 2.5.0 workspace and tested by `test_plate_layout.cpp` | Layout unit tests exist; full GUI operations require workstation verification |
| `.3mf` per-plate printer snapshots | Existing custom metadata `orca_plate_printer_profile_v1`; project round-trip test in `tests/libslic3r/test_3mf.cpp` | Confirms serializer behavior, not complete Save/As, GUI reload or all filament overrides |
| Per-plate restore before constructing print | `a90ec844b5bf` fix ported to this candidate | Restores the saved printer context before `get_print()` creates per-plate print state; native build and GUI round-trip still pending |
| Moonraker / Bambu routing and native filament controls | `27b854c6ae` integration, including `test_plate_print_context.cpp` | Earlier networking worked in one Manjaro test; mixed Bambu-project/Klipper routing remains unreliable in later reports, so not signed off |
| Native Manjaro incremental build method | Previously completed with a user systemd scope, `MemorySwapMax=0` and `--parallel 2` | Build procedure, not an application functionality fix |

## Not yet fixed / release blockers

- Unexpected supports inside solid model geometry: no reproduced minimal project, root cause, or verified fix; do not claim this candidate resolves it.
- Per-plate process/filament Save, Save As, delete, editor and color controls: runtime parity is not established.
- Mixed Bambu+Moonraker host/device switching (including two Moonraker plates) must be tested.
- Full Save → exit → reopen → compare all plate/row assignments, process/filament settings, bed types, objects, and slicing remains a blocking test.
- Full GUI and slicing tests on both belt and conventional printers remain blocking.
- Keep Windows ARM64 out of any eventual release; Windows x64 only.

## Required recovery validation

1. Build this *pinned branch* in an isolated directory, using the tested user-scope memory limits and no destructive source cleanup.
2. Start with a separate `--datadir` copied from the target profile; keep the prior executables and user projects intact.
3. Create two printer rows (Bambu and Klipper), two plates in each; change printer, process, filament, bed type and support settings independently.
4. Save a **new** `.3mf`, close the application, reopen it, and verify exact row, plate and object placement as well as print/filament profile names and support settings.
5. Slice a closed solid cube with support disabled and a known overhang model with support enabled, inspect layer/toolpath preview and exported G-code. Compare against upstream Orca with identical model and effective settings. Never print a failing slice.
6. Verify separate Moonraker hosts and Bambu routing, including Bambu-origin projects with Klipper-assigned plates and two Klipper destinations.
7. Only after these pass, build and package a release pinned to the tested commit; replace desktop launchers without deleting user profiles.

These items are the acceptance criteria; they are **not assertions that the candidate already passes**.
