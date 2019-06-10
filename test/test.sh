#!/bin/bash
try() {
    expected="$1"
    input="$2"

    dist/ccpr "$input" > tmp/tmp.s
    gcc -o tmp/tmp tmp/tmp.s
    tmp/tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else    
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 2 '5 - 3'
try 41 " 12 + 34 - 5 "

echo OK