#!/bin/bash

set -e

ACTUAL_OUTPUT=$(timeout 5 curl -k https://127.0.0.1:8888/index.html | \
  tr -d '[:space:]')
EXPECTED_OUTPUT=$(echo "<!DOCTYPE html>
<html xmlns:th=\"https://www.thymeleaf.org\">
  <head>
    <meta charset=\"utf-8\" />
    <title>Dummp Page</title>
  </head>
  <body>
    Hello World!!!
  </body>
  </html>" | tr -d '[:space:]')

EXIT_STATUS=0
if diff <(echo "${ACTUAL_OUTPUT}") <(echo "${EXPECTED_OUTPUT}");
then
  echo "Service does function as expected."
else
  echo "Service does NOT function as expected."
  echo "${ACTUAL_OUTPUT} != ${EXPECTED_OUTPUT}"
  EXIT_STATUS=500
fi

TO_KILL=$(ps aux | grep https_dag_scheduler | grep -v "grep" \
  | awk '{print $2}')
echo "Going to kill executable --${TO_KILL}--..."
set +e
kill -9 ${TO_KILL}

TO_KILL=$(ps aux | grep dag_scheduler_service_startup | grep -v "grep" \
  | awk '{print $2}')
echo "Going to kill script --${TO_KILL}--..."
set +e
kill -9 ${TO_KILL}

echo "Exiting test with EXIT_STATUS=${EXIT_STATUS}."
exit ${EXIT_STATUS}
