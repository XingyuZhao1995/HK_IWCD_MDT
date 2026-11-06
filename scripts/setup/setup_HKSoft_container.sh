#!/bin/bash

# Source environment
source /usr/local/hk/hk-pilot/setup.sh
source /usr/local/hk/hk-software/install-Linux_x86_64-gcc_8-python_3.8.13/setup.sh 

# Source WCSim
source /vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/WCSim/install/bin/this_wcsim.sh

# Source MDT
MDTPath="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/install/bin"
export PATH=${MDTPath}:${PATH:-}
