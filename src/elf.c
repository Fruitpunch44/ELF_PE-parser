#include"my_elf.h"

/* Return machine architecture name */
const char *elf_machine_name(uint16_t machine_num) {
    switch (machine_num) {
        case EM_386:    return "Intel 386";
        case EM_X86_64: return "AMD x86-64";
        case EM_ARM:    return "ARM";
        case EM_RISCV:  return "RISC-V";
        default:        return "Unknown machine type";
    }
}
/*Return bind time*/
const char *elf_bind_type(uint16_t bind_type){
    switch(bind_type){
        case STB_GLOBAL: return "Global";
        case STB_LOCAL:  return "Local";
        case STB_WEAK:   return "Weak";
        case STB_NUM:    return "Num";
        default :        return "I haven't mapped that shit yet";
    }

}
/*Retrun sym type*/
const char *elf_sym_type(uint16_t sym_type){
    switch(sym_type){
        case STT_FILE:     return "FILE";
        case STT_NOTYPE:   return "NOTY";
        case STT_FUNC:     return "FUNC";
        case STT_SECTION:  return "SECT";
        case STT_OBJECT:   return "OBJT";
    }
}

/* Return OS/ABI name */
const char *elf_osabi_name(uint8_t osabi) {
    switch (osabi) {
        case ELFOSABI_SYSV:       return "UNIX - System V";
        case ELFOSABI_HPUX:       return "HP-UX";
        case ELFOSABI_NETBSD:     return "NetBSD";
        case ELFOSABI_LINUX:      return "Linux";
        case ELFOSABI_SOLARIS:    return "Solaris";
        case ELFOSABI_AIX:        return "AIX";
        case ELFOSABI_IRIX:       return "IRIX";
        case ELFOSABI_FREEBSD:    return "FreeBSD";
        case ELFOSABI_TRU64:      return "TRU64 UNIX";
        case ELFOSABI_MODESTO:    return "Novell Modesto";
        case ELFOSABI_OPENBSD:    return "OpenBSD";
        case ELFOSABI_ARM:        return "ARM";
        case ELFOSABI_STANDALONE: return "Standalone (embedded)";
        default:                  return "Unknown OS/ABI";
    }
}

/* Return ELF class (32/64-bit) */
const char *elf_class(uint8_t elf_class) {
    switch (elf_class) {
        case ELFCLASS32:   return "ELF32";
        case ELFCLASS64:   return "ELF64";
        case ELFCLASSNONE: return "Invalid class";
        default:           return "Not registered";
    }
}

/* Return data encoding (endianness) */
const char *elf_data_format(uint16_t elf_data) {
    switch (elf_data) {
        case ELFDATANONE: return "Unknown data format";
        case ELFDATA2LSB: return "Two's complement, little-endian";
        case ELFDATA2MSB: return "Two's complement, big-endian";
        default:          return "Not registered";
    }
}

/* Return ELF file type */
const char *elf_file_type(uint16_t elf_type) {
    switch (elf_type) {
        case ET_NONE: return "No file type";
        case ET_REL:  return "Relocatable file";
        case ET_EXEC: return "Executable file";
        case ET_DYN:  return "Shared object file";
        case ET_CORE: return "Core file";
        default:      return "Invalid file type";
    }
}

/* Return ELF version string */
const char *elf_version_name(uint16_t version) {
    switch (version) {
        case EV_NONE:    return "Invalid version";
        case EV_CURRENT: return "Current version";
        default:         return "Unknown version";
    }
}

/*parse Symbol tabel helper func*/
void parse_symbol_table(FILE *file,ElfW(Shdr) *section_headers,ElfW(Shdr) *string_table){
    // Number of symbols
    int num_syms = section_headers->sh_size / sizeof(ElfW(Sym));

    // Allocate space for symbols
    ElfW(Sym) *symbols = malloc(section_headers->sh_size);
    if (!symbols) {
        perror("unable to allocate space for symbols");
        exit(EXIT_FAILURE);
    }

    // Read symbols
    fseek(file, section_headers->sh_offset, SEEK_SET);
    fread(symbols, sizeof(ElfW(Sym)), num_syms, file);

    // Allocate space for string table
    char *strtab = malloc(string_table->sh_size);
    if (!strtab) {
        perror("unable to allocate space for string table");
        exit(EXIT_FAILURE);
    }

    // Read string table
    fseek(file, string_table->sh_offset, SEEK_SET);
    fread(strtab, string_table->sh_size, 1, file);

    // Print symbols
    for (int i = 0; i < num_syms; i++) {
        if (symbols[i].st_name != 0) {
            fprintf(stdout,"NUM: %d  Value: %8x  Size: %lu  Type: %s  Bind: %s  NDX:  %lu  Name:%s \n",
            i,
            symbols[i].st_value,
            symbols[i].st_size,
            elf_sym_type(symbols[i].st_info),
            elf_bind_type(symbols[i].st_info),
            symbols[i].st_shndx,
            strtab + symbols[i].st_name);
        }
    }

    free(symbols);
    free(strtab);
}

/*parse section table helper func*/
void parse_section_table(FILE *file,ElfW(Shdr) *section_headers,ElfW(Ehdr)*header){
    //getting a segfault
     char *Section_names=malloc(section_headers->sh_size);
        if(!Section_names){
            perror("unable to allocate memory");
            exit(EXIT_FAILURE);
        }       
        fseek(file, section_headers->sh_offset, SEEK_SET);
        fread(Section_names, section_headers->sh_size, 1, file);
        printf("Section Headers:\n");
        for(int idx =0; idx < header->e_shnum;++idx){
            const char *name= "";
            fseek(file,header->e_shoff+idx*sizeof(ElfW(Shdr)),SEEK_SET);
            name=Section_names+section_headers[idx].sh_name;
            printf("Section names:\n");
            printf("%i %s\n",idx,name);
        }
        free(Section_names);
}

/*return both the symbol and sections names*/
void parse_symbol_and_sections_table(const char *elf_file) {
    ElfW(Ehdr) elf_header;
    FILE *file = fopen(elf_file, "rb");
    if (!file) {
        perror("failed to open ELF file");
        exit(EXIT_FAILURE);
    }

    // read ELF header
    if (fread(&elf_header, sizeof(elf_header), 1, file) != 1) {
        perror("failed to read ELF header");
        exit(EXIT_FAILURE);
    }
    fseek(file, elf_header.e_shoff, SEEK_SET);

    // read ALL section headers into array
    fseek(file, elf_header.e_shoff+elf_header.e_shstrndx*elf_header.e_shentsize, SEEK_SET);

    ElfW(Shdr) *section_headers = malloc(sizeof(ElfW(Shdr)) * elf_header.e_shnum);
    if (!section_headers) {
        perror("malloc section headers");
        exit(EXIT_FAILURE);
    }

    fread(section_headers,sizeof(ElfW(Shdr)),1, file);

  
    parse_section_table(file, section_headers,&elf_header);

    // loop through sections and parse symbol names
    for (int i = 0; i < elf_header.e_shnum; ++i) {
        if (section_headers[i].sh_type == SHT_SYMTAB ||section_headers[i].sh_type == SHT_DYNSYM) {
            
            ElfW(Shdr) *symtab_hdr = &section_headers[i];
            ElfW(Shdr) *strtab_hdr = &section_headers[symtab_hdr->sh_link];

            parse_symbol_table(file, symtab_hdr, strtab_hdr);
        }
    }

    free(section_headers);
    fclose(file);
}


void parse_text_section(const char* elf_file){
    ElfW(Ehdr) header;
    FILE *file=fopen(elf_file,"rb");
    if(!file){
        perror("could not read the elf file");
        exit(1);
    
    }
    if(file){
        //read header first
        if (fread(&header, sizeof(header), 1, file) != 1) {
            perror("failed to read ELF header");
            exit(EXIT_FAILURE);
        }
        fseek(file,header.e_shoff,SEEK_SET);

        //read all section headers
        ElfW(Shdr) *shdrs = malloc(header.e_shnum * sizeof(ElfW(Shdr)));
        if(!shdrs){
            perror("unable to allocate memory");
            exit(EXIT_FAILURE);
        }
        fread(shdrs, sizeof(ElfW(Shdr)), header.e_shnum, file);


        ElfW(Shdr) shstr = shdrs[header.e_shstrndx];
        fseek(file, shstr.sh_offset, SEEK_SET);
        char *shstrtab =malloc(shstr.sh_size);
        if(!shstrtab){
            perror("unable to allocate memory");
            exit(EXIT_FAILURE);
        }
        fread(shstrtab, shstr.sh_size, 1, file);

    

        for (int i = 0; i < header.e_shnum; i++){
            const char *name=shstrtab+ shdrs[i].sh_name;
            if (strcmp(name,".text")==0){
                printf("Section .text at offset %lu, size %lu\n",
               (unsigned long)shdrs[i].sh_offset,
               (unsigned long)shdrs[i].sh_size);
               fseek(file,shdrs[i].sh_offset,SEEK_SET);

               unsigned char *text=malloc(shdrs[i].sh_size);
               if(!text){
                perror("error in allocating memory");
                exit(1);
               }
               fread(text,shdrs[i].sh_size,1,file);
               printf("Machine Code \n");
                for (ssize_t j=0;j<shdrs[i].sh_size;j++){
                    printf("%02x ",text[j]);
                    if ((j+1) % 16 ==0){
                        printf("\n");
                    }
                }
                free(text);
    
            }
        }
        free(shdrs);
        free(shstrtab);
    } 
}

void print_elf_headers(const char* elf_file){
    ElfW(Ehdr) header;
    
    FILE* file=fopen(elf_file,"rb");
    if(!file){
        perror("unable to read file");
        exit(1);
    }

    if(file){
        //read header
        if (fread(&header, sizeof(header), 1, file) != 1) {
            perror("failed to read ELF header");
            exit(EXIT_FAILURE);
        }
        

        //elf file check
        if(memcmp(header.e_ident,ELFMAG,SELFMAG)==0){
            printf("%s is a valid ELF file\n", elf_file);

            printf("ELF Magic:   %02x %02x %02x %02x\n",
                header.e_ident[EI_MAG0], header.e_ident[EI_MAG1],
                header.e_ident[EI_MAG2], header.e_ident[EI_MAG3]);

            printf("Class:       %s\n", elf_class(header.e_ident[EI_CLASS]));
            printf("Data:        %s\n", elf_data_format(header.e_ident[EI_DATA]));
            printf("OS/ABI:      %s\n", elf_osabi_name(header.e_ident[EI_OSABI]));
            printf("Version:     %s\n", elf_version_name(header.e_version));
            printf("Machine:     %s\n", elf_machine_name(header.e_machine));
            printf("Type:        %s\n", elf_file_type(header.e_type));

            printf("Entry point address:            %#lx\n", header.e_entry);
            printf("Start of program headers:       %lu (bytes into file)\n", header.e_phoff);
            printf("Start of section headers:       %lu (bytes into file)\n", header.e_shoff);

            printf("Size of this header:            %u (bytes)\n", header.e_ehsize);
            printf("Size of program headers:        %u (bytes)\n", header.e_phentsize);
            printf("Number of program headers:      %u\n", header.e_phnum);
            printf("Size of section headers:        %u (bytes)\n", header.e_shentsize);
            printf("Number of section headers:      %u\n", header.e_shnum);

            printf("Flags:                          %#x\n", header.e_flags);
            printf("Section header string table index: %u\n", header.e_shstrndx);

        }
        else{
            printf("%s is not a valid elf file",elf_file);
            exit(1);
        }
        fclose(file);
    }

}
