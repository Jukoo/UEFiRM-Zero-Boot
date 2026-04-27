//SPDX-LICENSE-IDENTIFIER:GPL-3.0 
/*
 * Author Umar BA <jUmarB@protonmail.com> 
 */


#include <efi.h> 
#include <efilib.h> 
#include <stdlib.h> 

EFI_STATUS 
EFIAPI 
efi_main(EFI_HANDLE  imghdl,  EFI_SYSTEM_TABLE *systab)
{
  unsigned int efi_pstatus = EXIT_SUCCESS; 
  InitializeLib(imghdl , systab) ; 
  Print(L"hi folks \012") ; 
   

efi_eplg: 
  return efi_pstatus; 
}
