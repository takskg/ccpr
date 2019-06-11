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
try 41 "12+34 - 5"
try 52 "2 + (10 * 5)"
try 3 "5 - 8 / 4"
try 47 "5+6*7"
try 15 "5*(9-6)"
try 4 "(3+5)/2"
#単項演算子
try 5 "-10 + 15"
try 38 "50 + -12"
#比較演算子
try 1 "1 == 1"
try 0 "3 == 5"
try 1 "1 != 2"
try 0 "26 != 26"
try 1 "12 > 8"
try 0 "12 > 38"
try 0 "12 > 12"
try 1 "59 >= 29"
try 1 "64 >= 64"
try 0 "76 >= 128"
try 1 "7 < 76"
try 0 "87 < 34"
try 0 "12 < 12"
try 1 "34 <= 48"
try 1 "12 <= 12"
try 0 "874 <= 234"

echo OK