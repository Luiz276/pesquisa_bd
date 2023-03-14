#!/usr/bin/env bash

#Script that runs all source files and organizes all output files of the load generators

#Example: sh run.sh 127.0.0.1 6379 1000

#Variables
ip="${1:-"127.0.0.1"}"
port="${2:-6379}"
reqs="${3:-100}"

#Creating corresponding directories
mkdir -p output
mkdir -p output/cont_proc
mkdir -p output/cont_thread

#Running executable files
for i in $(seq 0 20 100); do

    ./bin/context_process 1 256 $i 30 $reqs $ip $port  # t threads com $(reqs) requisições
    #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
    mv **.csv ./output/cont_proc
    
    for t in $(seq 2 2 8); do
        #loop for thread number
        #echo $t
        ./bin/context_process $t 256 $i 30 $reqs $ip $port  # t threads com $(reqs) requisições
        #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
        mv **.csv ./output/cont_proc
    done

    ./bin/context_thread 1 256 $i 30 $reqs $ip $port  # t threads com $(reqs) requisições
    #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
    mv **.csv ./output/cont_thread

    for t in $(seq 2 2 8); do
        #loop for thread number
        #echo $t
        ./bin/context_thread $t 256 $i 30 $reqs $ip $port  # t threads com $(reqs) requisições
        #mv cont_proc_lat.csv cont_proc_tp.csv ./output/cont_proc
        mv **.csv ./output/cont_thread
    done
done

#Tar command on output directory
tar cvzf output.tar.gz output