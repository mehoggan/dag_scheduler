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
    "openssl" > ${SCRIPT_DIR}/openssl.configure.log 2>&1

  ./scripts/build.openssl.sh \
    "${BUILD_DIR}" \
    "openssl" > ${SCRIPT_DIR}/openssl.build.log 2>&1

  ./scripts/install.openssl.sh \
    "${BUILD_DIR}" \
    "openssl" > ${SCRIPT_DIR}/openssl.install.log 2>&1
}
build_openssl

function build_boostbeast {
  ./scripts/configure.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost" > ${SCRIPT_DIR}/boost.configure.log 2>&1

  ./scripts/build.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost" > ${SCRIPT_DIR}/boost.build.log 2>&1

  ./scripts/install.boostbeast.sh \
    "${BUILD_DIR}" \
    "boost" > ${SCRIPT_DIR}/boost.install.log 2>&1
}
build_boostbeast
