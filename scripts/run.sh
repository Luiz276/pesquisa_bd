#------------------------------------------------------------------------------------------
#
#       Deprecated script. Use the one located in the root of the repository
#
#------------------------------------------------------------------------------------------


#!/usr/bin/env bash

#Variables
ip=$1
port=$2

#Script that runs all source files and organizes all output files of the load generators
#Changing directory to location of executable files
cd ../bin;

#Running executable files
./context_process 4 256 50 30 100000 $ip $port  # 4 threads com 100 requisições
./context_thread 4 256 50 30 100000 $ip $port   # 4 threads com 100 requisições

#Creating corresponding directories
cd ..
mkdir -p output
cd output
mkdir -p cont_proc
mkdir -p cont_thread

#Moving output to respective directories
mv ../bin/cont_proc_lat.csv ./cont_proc
mv ../bin/cont_proc_tp.csv ./cont_proc
mv ../bin/cont_thread_lat.csv ./cont_thread
mv ../bin/cont_thread_tp.csv ./cont_thread