# Upstream OrcaSlicer workflow references

These are unchanged reference copies of OrcaSlicer's GitHub Actions workflows, taken from [`OrcaSlicer/OrcaSlicer`](https://github.com/OrcaSlicer/OrcaSlicer/tree/main/.github/workflows) on 2026-11-15.

**These are not active GitHub Actions workflows.** GitHub executes workflows only when YAML files are directly under `.github/workflows/`; this subfolder is for side-by-side comparison with MPW's existing workflows. Do not move these files into the root or expect them to run as-is: they reference other upstream workflows, custom actions, scripts, cache conventions and signing/release secrets that require adaptation to MPW.

| File | Upstream blob SHA |
| --- | --- |
| [build_all.yml](https://github.com/OrcaSlicer/OrcaSlicer/blob/main/.github/workflows/build_all.yml) | `3fb204ef42b679b2e026f57466f77b89cbfb7cbc` |
| [build_check_cache.yml](https://github.com/OrcaSlicer/OrcaSlicer/blob/main/.github/workflows/build_check_cache.yml) | `b7fdbcdc3d33220f3fa96a57f1177274b105ecae` |
| [build_deps.yml](https://github.com/OrcaSlicer/OrcaSlicer/blob/main/.github/workflows/build_deps.yml) | `3c778d03e67e158e44bab3be43d09296baf0e8c0` |
| [build_orca.yml](https://github.com/OrcaSlicer/OrcaSlicer/blob/main/.github/workflows/build_orca.yml) | `2b567c1f07f55935e2a460e1ac96a1904380d127` |

To activate an adapted workflow, create a corresponding YAML file **directly** in `.github/workflows/`, after reviewing references to companion workflows, signing credentials, platform scripts, dependency caching, and artifact names.
