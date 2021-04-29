//==============================================================================
// File:           BootLoader.h
// Compiler:       IAR Atmel AVR C/EC++ Compiler
// Output Size:    -
// Created:        01/20/11
//
// Description:    This file is generated by 'create' tool and is to be included
//                 in the 'bootldr' project. It contains the settings to
//                 configure the boot loader according to the configurations
//                 used in the encrypted file.
//==============================================================================


#ifndef BOOTLOADER_H
#define BOOTLOADER_H


#define PAGE_SIZE 4096
#define MEM_SIZE  24576
#define SHIFT_SIZE 4096
#define CRC_CHECK
#define SIGNATURE 0x90311325
#define BUFFER_SIZE 4200
#define INITIALVECTOR_HI 0x286CCD07
#define INITIALVECTOR_LO 0x6DE808D9
#define KEY_COUNT 3


#endif // BOOTLOADER_H
