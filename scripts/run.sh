#!/usr/bin/env bash

#Script that runs all source files and organizes all output files of the load generators
#Changing directory to location of executable files
cd ../src/redis_code;

#Running executable files
./context_process 4 256 50 30 100000  # 4 threads com 100 requisições
./context_thread 4 256 50 30 100000    # 4 threads com 100 requisições

#Creating corresponding directories
mkdir -p output
cd ../../output
mkdir -p cont_proc
mkdir -p cont_thread

#Moving output to respective directories
mv ../src/redis_code/cont_proc_lat.csv ./cont_proc
mv ../src/redis_code/cont_proc_tp.csv ./cont_proc
mv ../src/redis_code/cont_thread_lat.csv ./cont_thread
mv ../src/redis_code/cont_thread_tp.csv ./cont_thread