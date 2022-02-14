#!/bin/bash
set -e

pushd `dirname "${0}"`
BASEDIR=$(pwd)
popd > /dev/null

apply_patch() {
  PATCH_FILE=rapidjson.patch
  pushd "${1}/rapidjson-src"
  echo "Going to apply patch in ${1}/rapidjson-src..."
  git apply --ignore-space-change --ignore-whitespace "${BASEDIR}/${PATCH_FILE}"
  popd
}

apply_patch ${1}
