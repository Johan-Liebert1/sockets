#!/bin/bash

name=$1
dont_run=$2

fname_wo_ext=$(echo $name | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}')

mkdir -p bin

gcc lib/headers.c ./lib/readn.c ./lib/writen.c ./lib/sock_ntop.c $1 -l pthread -o "bin/$fname_wo_ext"

if [[ "$dont_run" != "no" ]]; then
    ./bin/$fname_wo_ext ${@:2}
fi
