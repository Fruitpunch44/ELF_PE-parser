#ifndef MY_ELF_H
#define MY_ELF_H
#include<elf.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//check the system type if it's 64 bit
//if not use 32 bit

#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif


void print_elf_headers(const char* elf_file);
void parse_text_section(const char* elf_file);
void parse_section_table(const char* elf_file);


#endif