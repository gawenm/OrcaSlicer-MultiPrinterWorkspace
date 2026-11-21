# MPW upstream integration plan

## Branch roles

- `release/mpw-tested-2026-11-13`: protected known-working MPW source, project-saving fix `a90ec844b5bf`.
- `build/mpw-packages`: package and installer CI development (independent of upstream merges).
- `integration/upstream-orca`: safe review base for probing official OrcaSlicer changes.
- `main`: leave untouched until a complete integration is approved.

## First step: non-destructive compatibility check

The `MPW upstream compatibility check` workflow runs on pushes to
`integration/upstream-orca`. It fetches the official upstream OrcaSlicer
`main`, attempts a merge on an ephemeral GitHub Actions runner, and uploads
`mpw-upstream-merge-report` listing any conflicting paths. It never pushes
upstream code, merges a pull request, or rewrites the tested MPW baseline.

An unsuccessful check means conflicts require manual source integration; it
does not imply that the MPW persistence fix is broken.

## Local integration on Manjaro

Use the existing tested source repository, **not** the installed Orca binary:

```bash
cd "$HOME/Development/orca-mpw-docker-test.id0BgX/source"
git status --short
# Stop if this prints uncommitted changes. Save them before switching branches.
git fetch origin
git switch --track -c integration/upstream-orca origin/integration/upstream-orca
git remote get-url upstream >/dev/null 2>&1 ||
  git remote add upstream https://github.com/OrcaSlicer/OrcaSlicer.git
git fetch upstream main
git switch -c integration/upstream-review-$(date +%Y%m%d)
git merge --no-commit --no-ff upstream/main
```

If conflicts occur, inspect `git status` and resolve them in the disposable
review branch. Use `git merge --abort` to cancel. Never reset the tested release
or use `git push --force` on it. Once a conflict-free build is achieved, test
plate/profile selection, filament/process controls, printer networking and 3MF
save/reopen, then commit and push the reviewed integration branch.

## Gate for release

1. Clean upstream integration or fully reviewed conflict resolutions.
2. Local native Linux build and MPW regression tests, including round-trip 3MF.
3. GitHub Actions Linux, Windows, macOS builds and their dependency jobs.
4. Install package on clean systems, launch, slice and save/reopen project.
5. Create a draft prerelease from a **fixed commit**; do not publish artifacts
   as stable until tests pass.

Do not put official upstream-only files into the local system-wide
`/opt/orca-slicer-nightly` package to perform source integration.
