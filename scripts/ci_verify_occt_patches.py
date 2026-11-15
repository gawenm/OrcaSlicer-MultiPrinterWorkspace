#!/usr/bin/env python3
"""Verify the exact pinned OCCT archive and both patches before building.

Default mode downloads the OCCT ZIP from deps/OCCT/OCCT.cmake, checks its
SHA-256, and stages only the two files modified by our patches.
--source accepts an existing clean OCCT tree for offline development.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]
RECIPE = ROOT / "deps/OCCT/OCCT.cmake"
FILES = (
    "src/ModelingAlgorithms/TKMesh/BRepMesh/BRepMesh_ModelPreProcessor.cxx",
    "src/ModelingData/TKBRep/BRepGraph/BRepGraph_LayerTopoSupplement.cxx",
)
PATCHES = (
    ROOT / "deps/OCCT/0001-BRepMesh-seam-pcurve-at-edge-parameter.patch",
    ROOT / "deps/OCCT/0002-msvc-arm64-owner-kind-optimizer.patch",
)


def pinned_archive() -> tuple[str, str]:
    cmake = RECIPE.read_text(encoding="utf-8")
    u = re.search(r"(?m)^\s*URL\s+(https://\S+\.zip)\s*$", cmake)
    h = re.search(r"(?m)^\s*URL_HASH\s+SHA256=([0-9a-fA-F]{64})\s*$", cmake)
    if not u or not h:
        raise RuntimeError("Expected a pinned OCCT archive URL and SHA-256")
    url = u.group(1)
    if not url.startswith("https://github.com/Open-Cascade-SAS/OCCT/archive/refs/tags/"):
        raise RuntimeError(f"Unexpected OCCT archive origin: {url}")
    return url, h.group(1).lower()


def verified_zip(temp: Path) -> Path:
    url, expected = pinned_archive()
    archive = temp / "occt.zip"
    digest = hashlib.sha256()
    size = 0
    request = urllib.request.Request(url, headers={"User-Agent": "OrcaSlicer-MPW-OCCT-CI"})
    print(f"Downloading pinned OCCT archive: {url}", flush=True)
    with urllib.request.urlopen(request, timeout=120) as response, archive.open("wb") as out:
        while True:
            chunk = response.read(1024 * 1024)
            if not chunk:
                break
            size += len(chunk)
            if size > 500 * 1024 * 1024:
                raise RuntimeError("OCCT archive exceeds the 500 MiB preflight limit")
            digest.update(chunk)
            out.write(chunk)
    actual = digest.hexdigest()
    if actual != expected:
        raise RuntimeError(f"OCCT archive SHA-256 mismatch: {actual} != {expected}")
    print(f"Pinned OCCT SHA-256 verified ({size} bytes)", flush=True)
    return archive


def stage_sources(dest: Path, source: Path | None) -> None:
    if source is not None:
        print("Offline source override: archive SHA-256 NOT checked", flush=True)
        for rel in FILES:
            path = dest / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source / rel, path)
        return
    archive = verified_zip(dest.parent)
    with zipfile.ZipFile(archive) as zf:
        members = zf.namelist()
        for rel in FILES:
            matches = [n for n in members if n.endswith("/" + rel) and not n.endswith("/")]
            if len(matches) != 1:
                raise RuntimeError(f"Expected one pinned source entry for {rel}, got {len(matches)}")
            path = dest / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            with zf.open(matches[0]) as inp, path.open("wb") as out:
                shutil.copyfileobj(inp, out)


def verify_patch(dest: Path, patch: Path) -> None:
    if not patch.is_file():
        raise RuntimeError(f"Missing OCCT patch: {patch}")
    for extra in (["--check"], []):
        cmd = ["git", "apply", *extra, "--whitespace=error", str(patch)]
        completed = subprocess.run(cmd, cwd=dest, text=True, capture_output=True)
        if completed.returncode != 0:
            raise RuntimeError(f"Failed {patch.name}: {completed.stdout}{completed.stderr}")
    print(f"PASS: {patch.name}", flush=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, help="Clean OCCT source root; disables archive SHA check")
    args = parser.parse_args()
    try:
        with tempfile.TemporaryDirectory(prefix="mpw-occt-preflight-") as temporary:
            source = Path(temporary) / "source"
            source.mkdir()
            stage_sources(source, args.source)
            for patch in PATCHES:
                verify_patch(source, patch)
        print("OCCT source and patch preflight PASSED", flush=True)
        return 0
    except (OSError, RuntimeError, subprocess.SubprocessError, zipfile.BadZipFile) as exc:
        print(f"OCCT preflight FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
