# MultiPrinterWorkspace scope and regression contract

**Status:** binding requirements for the MPW recovery branch, not a claim that implementation/tests pass.

## Strict feature boundary

MPW changes only **plate assignment**, **plate grouping/layout**, **plate-specific slicing context**, **plate-specific printer destination**, and **project save/restore** for these features.

Everything else stays upstream OrcaSlicer. Do not introduce replacements for the native printer/filament/process dropdowns, filament popouts, material editor, printer editor, preset management, toolpath/support algorithms, preview, or unrelated interface functionality.

## Functional requirements

1. Each plate is assigned one named printer preset, process preset, filament slot assignments, bed type, and independent plate-local overrides.
2. Plates display in rows grouped by printer preset. A row can be collapsed; Add Plate adds a plate to the selected row; plates retain stable identity, row ordering, bed geometry and positions.
3. Saving an existing named printer preset updates all intended plates referencing it; Save As cannot silently rebind unrelated rows. Process and filament preset edits use Orca's native editor and propagate to intended references without erasing plate-local overrides.
4. The original printer dropdown, filament popout and edit menu, native Save/Save As/Delete/Revert/Undo controls remain available with correct action ownership for the selected plate. **Never globally enable a button without proving its action targets the selected plate/preset.**
5. The selected plate's fully resolved printer + process + filament + overrides config is passed into background slicing and export. No global printer/profile leakage across plates; the support engine is upstream, unchanged by MPW.
6. Device, network and print-host routing resolves from the selected plate's printer preset; no global Bambu/Moonraker fallback to another plate's destination.
7. Save as `.3mf`, close and reopen must restore all assignments, layout, bed data and overrides, including missing-preset handling. Do not silently replace an unavailable printer.
8. No destructive conversion of original Orca configuration directories or users' existing `.3mf` files.

## Known source-level violations / unverified behavior

- `src/slic3r/GUI/Tab.cpp`: `TabPrint::set_plate_process_config()` currently disables native Save, Delete and Undo while a plate config is displayed. Needs a safe plate-context Save/Save As/Revert integration, not an unconditional enable.
- `src/slic3r/GUI/Plater.cpp`: plate-mode sidebar switches to separate filament controls and disables certain native device/filament actions and color editing. Restore normal Orca affordances while keeping per-plate routing; do not simply remove safeguards.
- Existing per-plate 3MF serializer and loader need a full GUI round-trip test; the project-format unit test alone is insufficient.
- Unexpected generated supports inside solid geometry: investigate the **resolved config** and object/volume modifiers for the affected plate first; only change support algorithms on independent reproduced evidence of an upstream bug.

## Acceptance suite before replacing a workstation install

- Multi-printer row/plate layout and 3MF round-trip, including two plates per printer and independent process/filament/bed types.
- Normal native printer and filament popup navigation; edit, Save and Save As work on exactly the intended preset/plate.
- Two-printer slicing comparison with known-good Orca using identical geometry and effective config, with a solid closed primitive and a supported overhang test.
- Network destination tests for Bambu plate, Klipper plate, and a Bambu-origin project containing a Klipper-assigned plate.
- Verify original projects/config are preserved, then install an immutable tested binary and consolidate launchers.
