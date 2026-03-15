#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
APPDIR="${ROOT_DIR}/AppDir"
TOOLS_DIR="${ROOT_DIR}/tools"
DIST_DIR="${ROOT_DIR}/dist"
QT_PREFIX_DEFAULT="${HOME}/Qt/6.10.2/gcc_64"
QT_PREFIX="${QT_PREFIX:-${QT_PREFIX_DEFAULT}}"
LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
QT_PLUGIN="${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
APPIMAGETOOL="${TOOLS_DIR}/appimagetool-x86_64.AppImage"
ARCH="$(uname -m)"
RUNTIME_FILE="${TOOLS_DIR}/runtime-${ARCH}"
VERSION="$(grep -oPm1 '^project\(NOX VERSION \K[^ )]+' "${ROOT_DIR}/CMakeLists.txt" || true)"

mkdir -p "${BUILD_DIR}" "${APPDIR}" "${TOOLS_DIR}" "${DIST_DIR}"

if [[ ! -x "${LINUXDEPLOY}" || ! -x "${QT_PLUGIN}" || ! -x "${APPIMAGETOOL}" ]]; then
  echo "Missing AppImage tooling in ${TOOLS_DIR}."
  echo "Download linuxdeploy, linuxdeploy-plugin-qt, and appimagetool first."
  exit 1
fi

if [[ -z "${VERSION}" ]]; then
  echo "Unable to determine project version from CMakeLists.txt."
  exit 1
fi

if [[ ! -f "${RUNTIME_FILE}" ]]; then
  echo "Missing AppImage runtime file: ${RUNTIME_FILE}"
  echo "Download it from https://github.com/AppImage/type2-runtime/releases"
  exit 1
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_PREFIX_PATH="${QT_PREFIX}"
env QTFRAMEWORK_BYPASS_LICENSE_CHECK=1 cmake --build "${BUILD_DIR}" -j"$(nproc)"

rm -rf "${APPDIR}"
cmake --install "${BUILD_DIR}" --prefix "${APPDIR}/usr"

cp "${ROOT_DIR}/resources/icons/nox.svg" "${APPDIR}/nox.svg"
cp "${BUILD_DIR}/NOX.desktop" "${APPDIR}/NOX.desktop"

export QMAKE="${QT_PREFIX}/bin/qmake"
export LD_LIBRARY_PATH="${QT_PREFIX}/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="${QT_PREFIX}/plugins"
export QML_SOURCES_PATHS="${ROOT_DIR}/ui/qml"
export EXTRA_QT_PLUGINS="wayland-decoration-client;wayland-graphics-integration-client;wayland-shell-integration"
export PATH="${TOOLS_DIR}:${PATH}"
export APPIMAGE_EXTRACT_AND_RUN=1

ln -sf "${QT_PLUGIN}" "${TOOLS_DIR}/linuxdeploy-plugin-qt"
ln -sf "${APPIMAGETOOL}" "${TOOLS_DIR}/appimagetool"

"${LINUXDEPLOY}" \
  --appdir "${APPDIR}" \
  --desktop-file "${BUILD_DIR}/NOX.desktop" \
  --icon-file "${ROOT_DIR}/resources/icons/nox.svg" \
  --plugin qt

"${APPIMAGETOOL}" \
  --no-appstream \
  --runtime-file "${RUNTIME_FILE}" \
  "${APPDIR}" \
  "${DIST_DIR}/NOX-${VERSION}-${ARCH}.AppImage"

echo "AppImage created in ${DIST_DIR}:"
ls -1 "${DIST_DIR}"/*.AppImage
