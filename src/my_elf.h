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
const char *elf_machine_name(uint16_t machine_num);
const char *elf_osabi_name(uint8_t osabi);
const char *elf_class(uint8_t elf_class);
const char *elf_data_format(uint16_t elf_data);
const char *elf_file_type(uint16_t elf_type);
const char *elf_version_name(uint16_t version);
const char *elf_bind_type(uint16_t bind_type);
const char *elf_sym_type(uint16_t sym_type);



#endif