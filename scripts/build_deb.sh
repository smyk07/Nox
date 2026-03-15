#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
DIST_DIR="${ROOT_DIR}/dist"
QT_PREFIX_DEFAULT="${HOME}/Qt/6.10.2/gcc_64"
QT_PREFIX="${QT_PREFIX:-${QT_PREFIX_DEFAULT}}"

mkdir -p "${DIST_DIR}"

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_PREFIX_PATH="${QT_PREFIX}"
env QTFRAMEWORK_BYPASS_LICENSE_CHECK=1 cmake --build "${BUILD_DIR}" -j"$(nproc)"
cpack --config "${BUILD_DIR}/CPackConfig.cmake" -G DEB

find "${ROOT_DIR}" -maxdepth 2 -name '*.deb' \
  ! -path "${DIST_DIR}/*" \
  -exec mv -f {} "${DIST_DIR}/" \;

echo "DEB package created in ${DIST_DIR}:"
ls -1 "${DIST_DIR}"/*.deb
