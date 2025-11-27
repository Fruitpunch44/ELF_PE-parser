#ifndef PE_H
#define PE_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<windows.h>

void *load_pe (char *PE_DATA,const char* FILE_NAME);
void load_exe_file(const char *file_path);
void parse_dos_header(IMAGE_DOS_HEADER *Dos_header);
void print_FILE_HEADERS(IMAGE_NT_HEADERS* HEADER);
void parse_optional_heders(IMAGE_NT_HEADERS* HEADER);
void parse_section_headers(IMAGE_NT_HEADERS *NT);





#endif