#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
APPDIR="${ROOT_DIR}/AppDir"
STAGE_DIR="${ROOT_DIR}/deb-stage"
TOOLS_DIR="${ROOT_DIR}/tools"
DIST_DIR="${ROOT_DIR}/dist"
QT_PREFIX_DEFAULT="${HOME}/Qt/6.10.2/gcc_64"
QT_PREFIX="${QT_PREFIX:-${QT_PREFIX_DEFAULT}}"
LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
QT_PLUGIN="${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
ARCH="$(uname -m)"
VERSION="$(grep -oPm1 '^project\(NOX VERSION \K[^ )]+' "${ROOT_DIR}/CMakeLists.txt" || true)"

case "${ARCH}" in
  x86_64)
    DEB_ARCH="amd64"
    ;;
  aarch64)
    DEB_ARCH="arm64"
    ;;
  *)
    DEB_ARCH="${ARCH}"
    ;;
esac

mkdir -p "${BUILD_DIR}" "${APPDIR}" "${TOOLS_DIR}" "${DIST_DIR}"

if [[ ! -x "${LINUXDEPLOY}" || ! -x "${QT_PLUGIN}" ]]; then
  echo "Missing linuxdeploy tooling in ${TOOLS_DIR}."
  echo "Download linuxdeploy and linuxdeploy-plugin-qt first."
  exit 1
fi

if [[ -z "${VERSION}" ]]; then
  echo "Unable to determine project version from CMakeLists.txt."
  exit 1
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_PREFIX_PATH="${QT_PREFIX}"
env QTFRAMEWORK_BYPASS_LICENSE_CHECK=1 cmake --build "${BUILD_DIR}" -j"$(nproc)"

rm -rf "${APPDIR}" "${STAGE_DIR}"
cmake --install "${BUILD_DIR}" --prefix "${APPDIR}/usr"

export QMAKE="${QT_PREFIX}/bin/qmake"
export LD_LIBRARY_PATH="${QT_PREFIX}/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="${QT_PREFIX}/plugins"
export QML_SOURCES_PATHS="${ROOT_DIR}/ui/qml"
export EXTRA_QT_PLUGINS="wayland-decoration-client;wayland-graphics-integration-client;wayland-shell-integration"
export PATH="${TOOLS_DIR}:${PATH}"
export APPIMAGE_EXTRACT_AND_RUN=1

ln -sf "${QT_PLUGIN}" "${TOOLS_DIR}/linuxdeploy-plugin-qt"

"${LINUXDEPLOY}" \
  --appdir "${APPDIR}" \
  --desktop-file "${BUILD_DIR}/NOX.desktop" \
  --icon-file "${ROOT_DIR}/resources/icons/nox.svg" \
  --plugin qt

PKGROOT="${STAGE_DIR}/nox_${VERSION}_${DEB_ARCH}"
mkdir -p "${PKGROOT}/DEBIAN" "${PKGROOT}/opt/nox" "${PKGROOT}/usr/bin" \
  "${PKGROOT}/usr/share/applications" "${PKGROOT}/usr/share/icons/hicolor/scalable/apps" \
  "${PKGROOT}/usr/share/metainfo"

cp -a "${APPDIR}/usr/." "${PKGROOT}/opt/nox/"
ln -s "../../opt/nox/bin/nox" "${PKGROOT}/usr/bin/nox"
cp "${BUILD_DIR}/NOX.desktop" "${PKGROOT}/usr/share/applications/NOX.desktop"
cp "${ROOT_DIR}/resources/icons/nox.svg" "${PKGROOT}/usr/share/icons/hicolor/scalable/apps/nox.svg"
cp "${ROOT_DIR}/packaging/io.github.brsrk94.Nox.appdata.xml" "${PKGROOT}/usr/share/metainfo/NOX.appdata.xml"

cat > "${PKGROOT}/DEBIAN/control" <<EOF
Package: nox
Version: ${VERSION}
Section: utils
Priority: optional
Architecture: ${DEB_ARCH}
Maintainer: brsrk94 <brsrk94@users.noreply.github.com>
Depends: libx11-6, libx11-xcb1, libxcb1, libxkbcommon0, libxkbcommon-x11-0, libxcb-cursor0, libxcb-icccm4, libxcb-util1, libxcb-image0, libxcb-keysyms1, libxcb-randr0, libxcb-render-util0, libxcb-shm0, libxcb-sync1, libxcb-xfixes0, libxcb-shape0, libxcb-xkb1, libegl1, libgl1, libopengl0, libfontconfig1, libfreetype6, libglib2.0-0
Description: Keyboard-first Linux launcher and command palette
 NOX is a Raycast-inspired launcher for Linux built with C++, Qt 6, and QML.
 It bundles its Qt runtime so it can run on stock Ubuntu systems without a
 matching local Qt SDK installation.
Homepage: https://github.com/brsrk94/Nox
EOF

find "${DIST_DIR}" -maxdepth 1 -name 'nox_*.deb' -delete
dpkg-deb --build --root-owner-group "${PKGROOT}" "${DIST_DIR}/nox_${VERSION}_${DEB_ARCH}.deb"

echo "DEB package created in ${DIST_DIR}:"
ls -1 "${DIST_DIR}"/*.deb
