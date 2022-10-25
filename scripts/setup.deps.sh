#!/bin/bash
BUILD_TYPE=${1}

set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
BUILD_DIR="${SCRIPT_DIR}/../deps"

if [[ -e "${BUILD_DIR}/build" ]];
then
  rm -rf "${BUILD_DIR}/build"
fi

mkdir "${BUILD_DIR}/build"

function build_openssl {
  ./scripts/configure.openssl.sh \
    "${BUILD_DIR}" \
    "${BUILD_TYPE}" \
    "openssl"

  ./scripts/build.openssl.sh \
    "${BUILD_DIR}" \
    "openssl"

  ./scripts/install.openssl.sh \
    "${BUILD_DIR}" \
    "openssl"
}
build_openssl

function build_boostbeast {
  ./scripts/configure.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost"

  ./scripts/build.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost"

  ./scripts/install.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost"
}
build_boostbeast
