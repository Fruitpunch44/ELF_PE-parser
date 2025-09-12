#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include "elf.c"

int main(int argc, char* argv[]){
    /*
    enum{
        PARSE_SECTION_HEADER,
        PARSE_SECTION_TABLE,
        PARSE_TEXT_SECTION,
        PARSE_ALL
    }mode;
    was usless i just wanted to add this cause i thought it looked cool lol
    */

    int option;
    if(argc<3){
        printf("pls pass in the required arguments\n");
        exit(1);
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

            default:
            fprintf(stderr,"usage : %s [-PTS] [FILE]",argv[0]);
            exit(EXIT_FAILURE);
        }
  
    }    
    return 0;
}