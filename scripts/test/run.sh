#!/bin/bash

# Which program to run
Option=${1}

# Execute the program
if [ ${Option} = "1" ]
then

  appGenPileUpSpill ./config.txt

elif [ ${Option} = "2" ]
then

  Input="/vols/hyperk/hk-neut.5.6.4.4-1_p7c1/IWCD_geom_v3_hz4930_r4640/wcsim_v1.12.35_cwcs1.1.1.1/ID/flux23av1_p320_nd25/rwcs/beam_neut_iwcd_p320_nd25_00000000-0010_rwcs.root"
  Output="test.root"
  ConfigFile="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/parameter/MDTParamenter_IWCD.txt"

  appIWCDSingleEvent -i ${Input} -p ${ConfigFile} -o ${Output} -s 1 -od -n -1

fi
