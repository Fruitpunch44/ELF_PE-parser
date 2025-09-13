#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include "elf.c"

int main(int argc, char* argv[]){
    int option;
    if(argc<3){
        fprintf(stderr,"usage : %s [-PTS] [FILE]",argv[0]);
        exit(EXIT_FAILURE);
    }
    char *file_name=argv[2];

    //parse command line args,man i miss python wtf is this
    //LINUX specific
    while((option = getopt(argc,argv,"PTSA"))!=1){
        switch(option){

            case 'P':
                print_elf_headers(file_name);
                break;

            case 'T': 
                parse_text_section(file_name);
                break;

            case 'S':
                parse_section_table(file_name);
                break;

            case 'A':
                print_elf_headers(file_name);
                parse_section_table(file_name);
                parse_text_section(file_name);
                break;
        }
  
    }    
    return 0;
}