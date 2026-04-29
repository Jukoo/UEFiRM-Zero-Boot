#!/bin/bash
#- Qemu launch script 
#- Author  Umar Ba <jUmarB@protonmail.com> 
#+++++++++++++++++++++++++++++++++++++++++++++++++ 
set -o pipefail 
set -o errexit 

[[ -f .config ]] && source .config || {
  echo -e "Please Launch the Qemu script launcher using start script"
  exit 1
} 


declare BYTE_ARCH=${ARCH:4}  
[[ -n  ${BYTE_ARCH} ]]  && { 
   BYTE_ARCH=x${BYTE_ARCH}  
}|| BYTE_ARCH="x32"

readonly  BYTE_ARCH 
readonly  EDK2_FULL_PATH=${EDK2_RPATH}${BYTE_ARCH}/${OVMF_FD} 
readonly  IMG_DISK_FULL_PATH="${IMG_RPATH}${IMG_DISK}"
has_command(){
  local exit_code; 
  [[ -n `command -v $1` ]] || {
    exit_code=$?
    printf "command not available :%s\012" ,"${1}"
    exit ${exit_code} 
  }
}
[[ ! -f ${EDK2_FULL_PATH} ]] && {
  printf "EFI Development Kit II: [not found]\012"
  exit -1 
}

launch_qemu(){
   qemu-system-${ARCH}  -cpu qemu${ARCH:4} \
     -bios ${EDK2_FULL_PATH} \
     -drive file=${IMG_DISK_FULL_PATH},format=raw,if=ide \
     -net none
}

has_command  qemu-system-${ARCH} 
launch_qemu  
