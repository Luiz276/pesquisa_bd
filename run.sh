#!/usr/bin/env bash

#Variables
ip=$1
port=$2
reqs=1000

#Script that runs all source files and organizes all output files of the load generators

#Running executable files
./bin/context_process 4 256 50 30 $reqs $ip $port  # 4 threads com 100 requisições
./bin/context_thread 4 256 50 30 $reqs $ip $port   # 4 threads com 100 requisições

#Creating corresponding directories
mkdir -p output
cd output
mkdir -p cont_proc
mkdir -p cont_thread
cd ..
#Moving output to respective directories
mv cont_proc_lat.csv ./output/cont_proc
mv cont_proc_tp.csv ./output/cont_proc
mv cont_thread_lat.csv ./output/cont_thread
mv cont_thread_tp.csv ./output/cont_thread