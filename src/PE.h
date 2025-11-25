#ifndef PE_H
#define PE_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<windows.h>

void *load_pe (char *PE_DATA,const char* FILE_NAME);
void print_dos(IMAGE_DOS_HEADER* DOS_HEADER);
void print_FILE_HEADERS(IMAGE_NT_HEADERS* HEADER);
void parse_optional_heders(IMAGE_NT_HEADERS* HEADER);
void parse_section_headers(IMAGE_NT_HEADERS *NT);





#endif