#!/bin/bash

includes=""

if [ -e swc.1.gz ]; then
    echo "Overwriting swc.1.gz"
    rm -f swc.1.gz
fi

for file in "$@"; do
    includes=$includes" --include=$file"
done

help2man --help-option='-h full'$includes -N ./swc -o swc.1
gzip swc.1

