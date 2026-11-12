# Orca-MPW — open implementation work and release blockers

Status: **development / unverified**. This is a working backlog, NOT a statement that the fixes below have been implemented. Do not merge this branch into `main` until the required workstation tests pass.

## Required behavior

- Preserve the **native OrcaSlicer 2.5.0 appearance and original settings editors**; no replacement GUI.
- **One printer preset per printer row**. Every plate in a row shares that named machine preset. If two rows use the same named preset, saving it affects both; Save As must not silently reassign an unrelated row.
- Each plate independently owns **filament assignment, process selection, plate overrides, models, objects, bed type and slicing results**.
- A saved `.3mf` must reopen with the same rows, plate order, assignments, machine/process/material settings and object placement, even when local presets have since changed. Missing installed presets must not silently substitute a different printer.
- Gridfinity STL import must go to the **selected plate** without opening another Orca process.
- Release only reviewed public source. Never upload credentials, printer host URLs/private IPs, personal paths, production presets, model files, `.3mf`, G-code, local backups, binary build output or private Git history.

## P0 — functionality/regression blockers

| ID | Area | Evidence / current status | Acceptance test |
|---|---|---|---|
| MPW-001 | Process Save / Save As | **Confirmed disabled** in original workspace patch: `TabPrint::set_plate_process_config` sets `m_btn_save_preset->Enable(!config)`; the plate context bypasses normal `Tab::update_dirty`. | Open process settings for assigned plate; save a named process preset and Save As via original Orca flow. Verify correct plate, no global-preset contamination. |
| MPW-002 | Process Delete / Undo / Revert | **Confirmed disabled** in same plate-config code; undo event and dirty state also need correct ownership. | Delete/revert appropriate preset with Orca safeguards; undo/revert plate overrides without changing another plate. |
| MPW-003 | Filament pencil and editor | **Confirmed regression** in original workspace patch: plate-local pencil calls `ForceDropdownOpen()`. Repair is staged in QA updater but GUI outcome **unverified**. | Pencil opens native filament editor; Save / Save As works for selected slot. |
| MPW-004 | Filament colour and slot handling | Plate-local colour picker is explicitly disabled in original patch. Mixed/unsaved filament selections may have empty choices. | Colour editing, compatible filament selection and multi-slot assignments work; switching plates keeps each plate's selection. |
| MPW-005 | Shared printer preset Save / Save As | Original plate editor disabled saving. Row-preset updater exists but workstation build/behavior **unverified**. | Save a machine change; all plates referencing that printer update, other printer presets do not. Save As rebinds only intended assignment(s). |
| MPW-006 | Printer/filament/process dropdowns | Source introduces plate-specific choice lists and cache fast paths. Static updater addresses part of the stale-binding risk; runtime **unverified**. | Switching plates and rows never shows wrong/stale options; selections stick and do not modify the wrong plate. |
| MPW-007 | Per-plate process state | Plate process options are copied into local configurations and bypass normal dirty logic; propagating saved *named* process preset to referring snapshots **not implemented/verified**. | Plate override edits persist independently; saving an installed process preset affects intended users of that preset without erasing overrides. |
| MPW-008 | Per-plate filament preset updates | Propagating edits to a *named* saved filament preset across existing plate snapshots **not implemented/verified**. | Save filament temp/colour preset; verify correct slots, no overwrites of different filaments, no stale slicing values. |
| MPW-009 | `.3mf` full round trip | Serializer was modified, but multi-row save/reopen and portability **unverified**. | Save → close → reopen → compare row/plate identity, ordering, presets, overrides, beds, printer settings, objects and custom G-code. |
| MPW-010 | Per-plate slicing/export | Slicing context and background processing patched; workstation integration **unverified**. | Slice and export one plate from two incompatible printers. Generated files have correct machine limits, material temperatures, process values and printer destination. |

## P1 — integration and UI parity blockers

| ID | Area | Work required |
|---|---|---|
| MPW-011 | Nozzle/extruder controls | Verify/re-enable nozzle diameter and flow selectors, extruder variants and multi-material editing in assigned-row mode, using the correct machine context. |
| MPW-012 | Device, Connect, Sync, AMS | Replace disabled plate-mode actions with safe routing to the selected row's actual device, or show an explicit *unsupported* state where unavoidable. Never send a print to a global printer by mistake. |
| MPW-013 | Purge / flushing / mixing | Restore flushing volumes, purge and mixed-material controls without cross-row contamination. Verify belt-printer-specific purge behavior. |
| MPW-014 | Add Bed / row operations | Selected-row Add Bed patch is present, actual runtime **unverified**. Check add, delete, reorder, undo/redo, duplicate and empty rows; forbid orphaned plate assignments. |
| MPW-015 | Bed geometry / printer type | Test belt vs conventional beds, printable/excluded area, nozzle offset, Z height and multi-bed materials. Ensure changes invalidate slicing results. |
| MPW-016 | Gridfinity direct STL import | Plugin-host bridge and Python fallback patches exist, actual GUI import **unverified**. Verify selected plate/row receives model and no extra Orca instance launches. |
| MPW-017 | Missing/renamed presets | Explicit prompt/repair path when a project references absent printer/filament/process presets; never silently substitute defaults. Test Save As and two rows sharing one preset name. |
| MPW-018 | Native GUI parity | Verify printer/filament/process Save/Delete/Revert, buttons, disabled states, keyboard shortcuts, tooltips and full original option pages. Avoid broad `Enable(true)` without ownership and routing checks. |
| MPW-019 | Project dirty and undo | Every meaningful row/bed/preset/filament/process change must mark the project dirty and integrate with undo/redo; saving and reopening must be deterministic. |
| MPW-020 | Public-release privacy | Scan **exact staged files and reachable Git history** for secrets and private content. Do not publish live configurations or model/project artifacts; verify the exact source diff before publishing. |

## Build and testing requirements

- Keep one incremental build directory; avoid unnecessary full rebuilds. Configurable 2–8 jobs, starting with 2–4 when memory/disk pressure is high.
- A successful source transform or Python unit test is **not** proof of successful C++ compilation or GUI functionality.
- Required smoke test: create a two-printer project, edit/save machine/filament/process presets, generate/import Gridfinity STL, slice both printers, save `.3mf`, close and reopen, compare assignments and G-code output.
- Test shared-named-preset behavior when **two rows reference the same printer**, and when two plates in one row use **different processes/filaments**.
- Do not merge into `main` until C++ build, app startup, relevant tests, full GUI interactions, round-trip persistence and public privacy scan pass.


## Source branch

This branch applies the published workspace and dropdown refresh directly to the OrcaSlicer v2.5.0 source. The original package and review history are available in the [Orca-MPW repository](https://github.com/gawenm/Orca-MPW/pull/1). Additional workstation edits and standalone updater packages need reconciliation with this source branch.
