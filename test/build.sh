#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "require cpp"
    exit 0
fi

g++ -o test $1 -I../include -L../out_lib -lmzx -static -std=c++11
