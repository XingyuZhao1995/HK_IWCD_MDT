#!/bin/bash

# Input file path
InputNuPath="/vols/hyperk/hk-neut.5.6.4.4-1_p7c1/IWCD_geom_v3_hz4930_r4640/wcsim_v1.12.35_cwcs1.1.1.1/ID/flux23av1_p320_nd25/rwcs"
FileListPath="/vols/hyperk/users/xzhao/HK/IWCD/IWCDSoftware/MDT/scripts/run/file_list"

# Output file path
OutputPath="/vols/hyperk/users/xzhao/HK/IWCD/OutputFiles/MDT_output/pileup"

# Create file list directory if not exist
if [ ! -d ${FileListPath} ]
then
  mkdir -p ${FileListPath}
fi

# Delete existing file lists
rm -rf ${FileListPath}/*.txt

# How many files to be put in each list
NFilePerList=8
# How many jobs to be submitted
NJobs=100

# Loop over input files
FileCount=0
JobIndex=0
for File in ${InputNuPath}/*.root
do

  # Update file count
  FileCount=$(($FileCount + 1))

  # Save the input file path to the file list
  echo "${File}" >> ${FileListPath}/${JobIndex}.txt 

  # For every specific amount of files, put their paths into the same list
  if (($FileCount % $NFilePerList == 0))
  then
    # Copy config file to file list folder and modify options inside
    cp ./config.txt ${FileListPath}/config_${JobIndex}.txt
    ConfigFile="${FileListPath}/config_${JobIndex}.txt"
    # (Neutrino interaction files)
    NuIntFiles="${FileListPath}/${JobIndex}.txt"
    sed -E -i "s|(<[[:space:]]*ListIDNuIntFiles[[:space:]]*=[[:space:]]*)[^[:space:]>]+|\1${NuIntFiles}|" "${ConfigFile}"
    # (Background files)
    sed -E -i "s|(<[[:space:]]*ListBeamBkgFiles[[:space:]]*=[[:space:]]*)[^[:space:]>]+|\1${NuIntFiles}|" "${ConfigFile}"
    # (Output prefix)
    OutputPrefix="${OutputPath}/${JobIndex}"
    sed -E -i "s|(<[[:space:]]*OutFileNamePrefix[[:space:]]*=[[:space:]]*)[^[:space:]>]+|\1${OutputPrefix}|" "${ConfigFile}"
    # (Initial seed)
    sed -E -i "s|(<[[:space:]]*InitialSeed[[:space:]]*=[[:space:]]*)[^[:space:]>]+|\1$(($JobIndex + 1))|" "${ConfigFile}"

    # Update file list index
    JobIndex=$(($JobIndex + 1))

    # Exit if job number reached
    if [ ${JobIndex} -ge "${NJobs}" ]
    then
      break
    fi
  fi

done

# Delete existing output files
rm -rf ${OutputPath}/*.root

# A summary file to put input file lists
SummaryFile="file_list.txt"
if [ -f ${SummaryFile} ]
then
  rm -f ${SummaryFile}
fi

# Loop over input config lists
for List in ${FileListPath}/config*.txt
do
  echo "${List}" >> ${SummaryFile}
done

# Delete existing log files as well
rm -rf /vols/hyperk/users/xzhao/HK/IWCD/OutputFiles/MDT_output/logs/*
