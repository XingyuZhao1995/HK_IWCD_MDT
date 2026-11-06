#!/bin/bash

# Container paths
export APPTAINER_CACHEDIR="/vols/hyperk/users/xzhao/Containers/cache"
export APPTAINER_TMPDIR="/vols/hyperk/users/xzhao/Containers/tmp"

# Enter the container
cd /vols/hyperk/users/xzhao/Containers
apptainer shell --writable --bind /vols/hyperk hk-software-sandbox

# Source environment
. /usr/local/hk/hk-pilot/setup.sh
. /usr/local/hk/hk-software/install-Linux_x86_64-gcc_8-python_3.8.13/setup.sh 

# Source WCSim
source /vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/WCSim/install/bin/this_wcsim.sh

