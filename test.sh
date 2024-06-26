#!/bin/bash
set -ex

for i in {1..30}
do
    echo "---- Run Test #$i ..."
    ./mtfind input.txt "?ad" $i > output.txt
    diff output.txt test-output.txt
done

echo 'All Tests Ok'
