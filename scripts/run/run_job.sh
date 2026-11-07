#!/bin/bash

# Config file
ConfigFile=${1}

# Run script
RunScript="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/scripts/run/run_container.sh"

export APPTAINER_CACHEDIR="/vols/hyperk/users/xzhao/Containers/cache"
export APPTAINER_TMPDIR="/vols/hyperk/users/xzhao/Containers/tmp"

# Execute the program
cd /vols/hyperk/users/xzhao/Containers
apptainer exec --bind /vols/hyperk hk-software-sandbox bash -c "source ${RunScript} ${ConfigFile}"
