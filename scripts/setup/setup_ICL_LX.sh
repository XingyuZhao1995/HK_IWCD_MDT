#!/bin/bash

# Container paths
export APPTAINER_CACHEDIR="/vols/hyperk/users/xzhao/Containers/cache"
export APPTAINER_TMPDIR="/vols/hyperk/users/xzhao/Containers/tmp"

# Enter the container
cd /vols/hyperk/users/xzhao/Containers
apptainer shell --writable --bind /vols/hyperk hk-software-sandbox
