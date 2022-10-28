#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

SERVER_ROOT="${1}"

if [[ ! -e "${SERVER_ROOT}" ]]
then
  mkdir -p "${SERVER_ROOT}"
fi

cat <<EOF > "${SERVER_ROOT}/index.html"
<!DOCTYPE html>
<html xmlns:th="https://www.thymeleaf.org">
  <head>
    <meta charset="utf-8" />
    <title>Dummp Page</title>
  </head>
  <body>
    Hello World!!!
  </body>
</html>
EOF

cat <<EOF > "${SERVER_ROOT}/server.yml"
---
address: "127.0.0.1"
port: 8888
doc-root: "${SERVER_ROOT}"
threads: 1
pem: "${SERVER_ROOT}/server.pem"
EOF

cp "${SCRIPT_DIR}/../bin/resources/server.pem" "${SERVER_ROOT}/server.pem"

if [[ "${OSTYPE}" == "linux-gnu"* ]]; then
  export TOP_DIR=${SCRIPT_DIR}/../deps/build
  export LD_LIBRARY_PATH=${TOP_DIR}/boost/lib
  export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TOP_DIR}/openssl/lib64
fi

$"${SCRIPT_DIR}/../build/bin/https_dag_scheduler_runner" \
  --server_yaml ${SERVER_ROOT}/server.yml &
