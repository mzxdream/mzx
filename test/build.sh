#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "require cpp"
    exit 0
fi

g++ -Wall -g -rdynamic -o test $1 -I../include -L../lib -l:libmzx.a -lpthread -std=c++11
