#!/bin/bash

LCG_VERSION="LCG_97python3"
COMPILER="gcc8"
PLATFORM="centos7"
BINARY_TAG="x86_64-${PLATFORM}-${COMPILER}-opt"

export ITK_PATH=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
export ITK_INST_PATH=${ITK_PATH}/installed

export ITK_DATA_PATH=/home/adecmos/data
if [ -z ${ITK_DATA_PATH+x} ]; then
    export ITK_DATA_PATH=/home/adecmos/data
    canwrite=`python -c 'import os; print os.access(os.getenv("ITK_DATA_PATH"), os.W_OK);'`
    if [ $canwrite == "False" ]; then 
        echo "Default output path is not writable. Please define ITK_DATA_PATH"
        export ITK_DATA_PATH="/home/adecmos/data"
    fi
fi



LCGPATHS=(/sw/atlas/sw/lcg /cvmfs/sft.cern.ch/lcg)
for _cand in ${LCGPATHS[@]}; do
    if [ -d ${_cand}/releases/${LCG_VERSION} ]; then
        export LCG_PATH=${_cand}
        break
    fi
done
if [ -z ${LCG_PATH+x} ]; then echo "LCG release not found"; return 0; fi

echo "---------"
echo "LCG_PATH=${LCG_PATH}"
echo "LCG_VERSION=${LCG_VERSION}"
echo "BINARY_TAG=${BINARY_TAG}"
echo "---------"

LCG_RELEASE_PATH=${LCG_PATH}/releases/${LCG_VERSION}

#Setup GCC and CMAKE
#source ${LCG_PATH}/contrib/gcc/8.3.0/${BINARY_TAG}/setup.sh
#source ${LCG_RELEASE_PATH}/CMake/3.14.3/${BINARY_TAG}/CMake-env.sh

eval "`${LCG_PATH}/releases/lcgenv/latest/lcgenv -p ${LCG_RELEASE_PATH} -s bash ${BINARY_TAG} CMake`"
eval "`${LCG_PATH}/releases/lcgenv/latest/lcgenv -p ${LCG_RELEASE_PATH} -s bash -G ${BINARY_TAG} ROOT`"
	
