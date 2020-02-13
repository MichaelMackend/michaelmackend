#!/bin/bash

usage=$(echo "./mmnew.sh [cpp|python] [service|app] [target folder]")

if [ $# -ne 3 ]; then
    echo $usage
    exit
fi

language=$1
projtype=$2
target=$3

mkdir -p $target

cp -T -f -r templates/$language/$projtype/ $target

pushd $target
if [ -f ./mmconfigure.sh ]; then
./mmconfigure.sh
fi

ls

