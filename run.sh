#!/bin/bash

name=$1
dont_run=$2

fname_wo_ext=$(echo $name | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}')

mkdir -p bin

gcc -g -l pthread  lib/* rwlocks/rwlock_* $1 -o "bin/$fname_wo_ext"

if [[ "$dont_run" != "no" ]]; then
    ./bin/$fname_wo_ext ${@:2}
fi
