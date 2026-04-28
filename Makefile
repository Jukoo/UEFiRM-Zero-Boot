## SPDX-LICENSE-IDENTIFIER: GPL-3.0 
## A custom UEFI  makefile - 
## Author : Umar BA <jUmarB@protonmail.com> 
## -- This makefile is used to build UEFI application using GNU-EFI 
## -- Can be used for  both architecture  32 & 64 bits  
## NOTE:!:  Please make sure  you have all requirements setup ...
## INFO:+: https://wiki.osdev.org/UEFI#Downloading_UEFI_images 

CC=gcc 
LD=ld
OBJCOPY=objcopy
HOSTLIBS:=/usr/lib
ARCH=$(shell uname -m)
USING_C_RUNTIME_0_FILE:=$(HOSTLIBS)/crt0-efi-$(ARCH).o 
EFI_LIKER_SCRIPT:=$(HOSTLIBS)/elf_$(ARCH)_efi.lds 
EFI_HEADER_INC:=/usr/include/efi

CFLAGS=  -fno-stack-protector  -fpic -fshort-wchar  -mno-red-zone \
		 -DEFI_FUNCTION_WRAPPER -I $(EFI_HEADER_INC)

LDFLAGS= $(USING_C_RUNTIME_0_FILE) -nostdlib  -znocombreloc		 \
		 -T $(EFI_LIKER_SCRIPT) -shared   -Bsymbolic \
		 -L $(HOSTLIBS)  -l:libgnuefi.a  -l:libefi.a  

 
OBJFLAGS= --only-section=.text                       \
          --only-section=.sdata                      \
          --only-section=.data                       \
          --only-section=.rodata                     \
          --only-section=.dynamic                    \
          --only-section=.dynsym                     \
          --only-section=.rel                        \
          --only-section=.rela                       \
          --only-section=.reloc                      \
          --output-target=efi-app-$(ARCH)  



SRC=$(wildcard src/efi/*.c) 
OBJ=$(SRC:.c=.o) 
EFI_APP_PE:=prolog.efi

## Move generated file to build  directory  
##+ the build directory ll'be created automaticaly...  
BUILDIR:=build 
define  move2buildir
	$(shell test  -d  $(BUILDIR)  || mkdir $(BUILDIR)) 
	$(info Move  generated file to  build directory)
	mv $(1) $(BUILDIR) 
endef



all: $(EFI_APP_PE) 

$(EFI_APP_PE): $(SRC:.c=.so)  
	$(info CS $< => $@)
	@$(OBJCOPY)  $(OBJFLAGS) $< $@  
	@$(call  move2buildir,$@)   
	@$(call  move2buildir,$<)

%.o : %.c 
	$(info CC $< => $@)
	@$(CC)  $<  -c  $(CFLAGS) -o  $@  

%.so:  %.o 
	$(info LD $< => $@)
	@$(LD) $< $(LDFLAGS) -o  $@



.PHONY: clean mproper qemu

qemu: 
	$(info Launching  VM using Qemu) 
	@sudo ./script/start_vm.sh 

clean:  
	$(warning no rule for $@) 

mproper: clean 
	$(info Delete $(BUILDIR)) 
	@rm -r $(BUILDIR)
