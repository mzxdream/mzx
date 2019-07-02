#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "require cpp"
    exit 0
fi

out=test

if [ $# -ge 2 ]; then
    out=$2
fi

g++ -Wall -g -rdynamic -o $out.out $1 -I../include -L../lib -l:libmzx.a -lpthread -std=c++11
