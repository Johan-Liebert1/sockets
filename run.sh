#!/bin/bash

name=$1

mkdir -p bin

gcc lib/headers.c $1 -o "bin/out"
./bin/out ${@:2}
