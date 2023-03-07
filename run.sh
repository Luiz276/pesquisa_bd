#!/usr/bin/env bash

#Script that runs all source files and organizes all output files of the load generators

#Example: sh run.sh 127.0.0.1 6379 1000

#Variables
ip=$1
port=$2
reqs=$3

#Running executable files
./bin/context_process 4 256 50 30 $reqs $ip $port  # 4 threads com 100 requisições
./bin/context_thread 4 256 50 30 $reqs $ip $port   # 4 threads com 100 requisições

#Creating corresponding directories
mkdir -p output
mkdir -p output/cont_proc
mkdir -p output/cont_thread

#Moving output to respective directories
mv cont_proc_lat.csv ./output/cont_proc
mv cont_proc_tp.csv ./output/cont_proc
mv cont_thread_lat.csv ./output/cont_thread
mv cont_thread_tp.csv ./output/cont_thread

#Tar command on output directory
tar cvzf output.tar.gz output