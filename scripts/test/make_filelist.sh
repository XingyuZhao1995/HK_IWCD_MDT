#!/bin/bash

# Generate input file list
InputNuPath="/vols/hyperk/hk-neut.5.6.4.4-1_p7c1/IWCD_geom_v3_hz4930_r4640/wcsim_v1.12.35_cwcs1.1.1.1/ID/flux23av1_p320_nd25/rwcs"
InputBkgPath="/vols/hyperk/hk-neut.5.6.4.4-1_p7c1/IWCD_geom_v3_hz4930_r4640/wcsim_v1.12.35_cwcs1.1.1.1/ID/flux23av1_p320_nd25/rwcs"

NMaxInputFile=2

InputNuList="./input_nu.txt"
if [ -f "${InputNuList}" ]
then
  rm ${InputNuList}
fi
InputBkgList="./input_bkg.txt"
if [ -f "${InputBkgList}" ]
then
  rm ${InputBkgList}
fi

# (Neutrino input loop)
FileCount=0
for File in ${InputNuPath}/*.root
do
  echo "${File}" >> ${InputNuList}

  FileCount=$(($FileCount + 1))
  if [ "${FileCount}" -ge ${NMaxInputFile} ]
  then
    break
  fi
done

# (Background input loop)
FileCount=0
for File in ${InputBkgPath}/*.root
do
  echo "${File}" >> ${InputBkgList}

  FileCount=$(($FileCount + 1))
  if [ "${FileCount}" -ge ${NMaxInputFile} ]
  then
    break
  fi
done

# Delete existing output files
rm -rf /vols/hyperk/users/xzhao/HK/IWCD/OutputFiles/MDT_output/*.root
