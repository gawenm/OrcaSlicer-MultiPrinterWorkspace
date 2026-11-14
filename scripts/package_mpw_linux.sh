#!/usr/bin/env bash
set -euo pipefail

image=$(realpath "$1")
version=$2
revision=$3
output=$(realpath -m "$4")
[[ $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ && $revision =~ ^[0-9]+$ ]]
mkdir -p "$output"

work=$(mktemp -d)
trap 'rm -rf -- "$work"' EXIT
root="$work/package"
payload="$root/opt/orca-mpw"
mkdir -p "$payload" "$root/usr/bin" "$root/usr/share/applications"
chmod +x "$image"
(
    cd "$work"
    "$image" --appimage-extract > /dev/null
)
cp -a "$work/squashfs-root/." "$payload/"
test -x "$payload/AppRun"

cat > "$root/usr/bin/orca-mpw" <<'LAUNCHER'
#!/bin/sh
exec /opt/orca-mpw/AppRun "$@"
LAUNCHER
chmod 755 "$root/usr/bin/orca-mpw"
cat > "$root/usr/share/applications/orca-mpw.desktop" <<'DESKTOP'
[Desktop Entry]
Type=Application
Name=OrcaSlicer MPW
Exec=orca-mpw %F
Terminal=false
Categories=Graphics;3DGraphics;
DESKTOP

url=https://github.com/gawenm/OrcaSlicer-MultiPrinterWorkspace
cat > "$root/.PKGINFO" <<PKGINFO
pkgname = orca-mpw
pkgver = $version-$revision
pkgdesc = OrcaSlicer MultiPrinterWorkspace preview
url = $url
builddate = $(date +%s)
packager = Orca-MPW GitHub Actions
size = $(du -sb "$root" | cut -f1)
arch = x86_64
license = AGPL-3.0-only
depend = glibc>=2.39
depend = gtk3
depend = webkit2gtk-4.1
depend = libglvnd
depend = mesa
depend = nss
PKGINFO
tar --sort=name --owner=0 --group=0 --numeric-owner -C "$root" \
    --zstd -cf "$output/orca-mpw-$version-$revision-x86_64.pkg.tar.zst" .PKGINFO opt usr
rm "$root/.PKGINFO"

mkdir "$root/DEBIAN"
cat > "$root/DEBIAN/control" <<CONTROL
Package: orca-mpw
Version: $version-$revision
Section: graphics
Priority: optional
Architecture: amd64
Maintainer: Orca-MPW GitHub Actions
Homepage: $url
Depends: libc6 (>= 2.39), libgtk-3-0t64, libwebkit2gtk-4.1-0, libgl1, libegl1, libopengl0, libnss3
Description: OrcaSlicer MultiPrinterWorkspace preview
CONTROL
dpkg-deb --root-owner-group --build "$root" "$output/orca-mpw_${version}-${revision}_amd64.deb"
