#!/bin/bash

TMP=$(mktemp)
./wav >"${TMP}"
gnuplot -p -e "plot \"${TMP}\" using 1 with line , \"${TMP}\" using 2 with line"
rm "${TMP}"

