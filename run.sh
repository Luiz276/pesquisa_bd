#!/usr/bin/env bash

#Script that runs all source files and organizes all output files of the load generators

#Example: sh run.sh 127.0.0.1 6379 1000

#Variables
ip=$1
port=$2
reqs=$3

#Creating corresponding directories
mkdir -p output
mkdir -p output/cont_proc
mkdir -p output/cont_thread

#Running executable files
for t in $(seq 1 4); do
    echo $t
    ./bin/context_process $t 256 50 30 $reqs $ip $port  # t threads com $(reqs) requisições
    #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
    mv **.csv ./output/cont_proc
done

for t in $(seq 1 4); do
    echo $t
    ./bin/context_thread $t 256 50 30 $reqs $ip $port  # t threads com $(reqs) requisições
    #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
    mv **.csv ./output/cont_thread
done

#Tar command on output directory
tar cvzf output.tar.gz output