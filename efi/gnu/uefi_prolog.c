//SPDX-LICENSE-IDENTIFIER:GPL-3.0 
/*
 * Author Umar BA <jUmarB@protonmail.com> 
 */


#include <efi.h> 
#include <efilib.h> 

typedef EFI_SIMPLE_TEXT_OUT_PROTOCOL  efi_simple_text_oproto ;  

EFI_STATUS 
EFIAPI 
efi_main(EFI_HANDLE  imghdl,  EFI_SYSTEM_TABLE *systab)
{
  unsigned int efi_pstatus = EFI_SUCCESS ;  
  InitializeLib(imghdl , systab) ; 
  systab->ConOut->Reset(systab->ConOut , 1) ; 

  Print(L"Booting\012") ;

efi_eplg: 
  return efi_pstatus; 
}
