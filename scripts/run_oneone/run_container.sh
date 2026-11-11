#!/bin/bash

# File list
FileList=${1}

# Source environment
source /usr/local/hk/hk-pilot/setup.sh
source /usr/local/hk/hk-software/install-Linux_x86_64-gcc_8-python_3.8.13/setup.sh 

# Source WCSim
source /vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/WCSim/install/bin/this_wcsim.sh

# Source MDT
export MDTROOT=/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT
MDTPath="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/install/bin"
export PATH=${MDTPath}:${PATH:-}

# Config file
ConfigFile="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/parameter/MDTParamenter_IWCD.txt"

# Read file list line by line
while read -r Input Output Seed
do
  # Run the program
  appIWCDSingleEvent -i ${Input} -p ${ConfigFile} -o ${Output} -s ${Seed} -od -n -1
done < ${FileList}
