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
const char *Symbols_table_type(uint16_t sym_type){
    switch(sym_type){
        case SHT_DYNSYM: return "Dynamic Symbol Table";
        case SHT_SYMTAB: return "Static Symbol Table";
        default:         return "Unknown Symbol Table";
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
    ElfW(Sym) *symbols = malloc(string_table->sh_size);
    if (!symbols) {
        perror("unable to allocate space for symbols");
        exit(EXIT_FAILURE);
    }

    // Read symbols
    fseek(file, string_table->sh_offset, SEEK_SET);
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
    fprintf(stdout,"Symbol Table Type: %s\n",Symbols_table_type(section_headers->sh_type));
    fprintf(stdout,"Num:    Value:      Size:       Type:    Bind:    NDX:   Name:\n");
    for (int i = 0; i < num_syms; i++) {
        if (symbols[i].st_name != 0) {
            fprintf(stdout,"%-8d  %#.8x  %#.8x  %-8s  %-8s  %-8lu  %s\n",
            i,
            symbols[i].st_value,
            symbols[i].st_size,
            elf_sym_type(ELF32_ST_TYPE(symbols[i].st_info)||ELF64_ST_TYPE(symbols[i].st_info)),
            elf_bind_type(ELF32_ST_BIND(symbols[i].st_info)||ELF64_ST_BIND(symbols[i].st_info)),
            symbols[i].st_shndx,
            strtab + symbols[i].st_name);
        }
    }

    free(symbols);
    free(strtab);
}

/*parse section table helper func*/
void parse_section_table(FILE *file,ElfW(Shdr) *section_headers,ElfW(Ehdr)*header){
    //read string table
    ElfW(Shdr) *string_table = &section_headers[header->e_shstrndx];
     char *Section_names=malloc(string_table->sh_size);
        if(!Section_names){
            perror("unable to allocate memory");
            exit(EXIT_FAILURE);
        }       
        fseek(file, string_table->sh_offset, SEEK_SET);
        fread(Section_names, string_table->sh_size, 1, file);
        printf("Section Headers names:\n");
        for(int idx =0; idx < header->e_shnum;++idx){
            const char *name= "";
            fseek(file,header->e_shoff+idx*sizeof(ElfW(Shdr)),SEEK_SET);
            name=Section_names+section_headers[idx].sh_name;
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
    fseek(file, elf_header.e_shoff, SEEK_SET);

    ElfW(Shdr) *section_headers = malloc(sizeof(ElfW(Shdr)) * elf_header.e_shnum);
    if (!section_headers) {
        perror("malloc section headers");
        exit(EXIT_FAILURE);
    }

    fread(section_headers,sizeof(ElfW(Shdr)),elf_header.e_shnum, file);

  
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

void parse_text_section(const char* elf_file) {
    ElfW(Ehdr) elf_header;
    FILE *file = fopen(elf_file, "rb");
    if (!file) {
        perror("could not read the ELF file");
        exit(EXIT_FAILURE);
    }

    // Read ELF header
    if (fread(&elf_header, sizeof(elf_header), 1, file) != 1) {
        perror("failed to read ELF header");
        exit(EXIT_FAILURE);
    }

    // Read section headers
    fseek(file, elf_header.e_shoff, SEEK_SET);
    ElfW(Shdr) *section_headers = malloc(sizeof(ElfW(Shdr)) * elf_header.e_shnum);
    if (!section_headers) {
        perror("unable to allocate memory for section headers");
        exit(EXIT_FAILURE);
    }
    fread(section_headers, sizeof(ElfW(Shdr)), elf_header.e_shnum, file);

    // Read section header string table
    ElfW(Shdr) *shstrtab_hdr = &section_headers[elf_header.e_shstrndx];
    char *shstrtab = malloc(shstrtab_hdr->sh_size);
    if (!shstrtab) {
        perror("unable to allocate memory for shstrtab");
        exit(EXIT_FAILURE);
    }
    fseek(file, shstrtab_hdr->sh_offset, SEEK_SET);
    fread(shstrtab, shstrtab_hdr->sh_size, 1, file);

    // Locate .text, .symtab, .strtab
    ElfW(Shdr) *text_section = NULL;
    ElfW(Shdr) *symtab_hdr = NULL;
    ElfW(Shdr) *strtab_hdr = NULL;
    for (int i = 0; i < elf_header.e_shnum; i++) {
        const char *section_name = shstrtab + section_headers[i].sh_name;
        if (strcmp(section_name, ".text") == 0) {
            text_section = &section_headers[i];
        } else if (strcmp(section_name, ".symtab") == 0) {
            symtab_hdr = &section_headers[i];
        } else if (strcmp(section_name, ".strtab") == 0) {
            strtab_hdr = &section_headers[i];
        }
    }
    if (!text_section) {
        fprintf(stderr, ".text section not found\n");
        exit(EXIT_FAILURE);
    }
    if (!symtab_hdr || !strtab_hdr) {
        fprintf(stderr, "symbol table or string table not found\n");
        exit(EXIT_FAILURE);
    }

    // Read .text section
     char *text_data = malloc(text_section->sh_size);
    if (!text_data) {
        perror("unable to allocate memory for .text section");
        exit(EXIT_FAILURE);
    }
    fseek(file, text_section->sh_offset, SEEK_SET);
    fread(text_data, text_section->sh_size, 1, file);

    // Dump .text section
    printf(".text section (size: %lu bytes):\n", (unsigned long)text_section->sh_size);
    for (size_t i = 0; i < text_section->sh_size; i++) {
        if (i % 16 == 0) printf("\n%08zx  ", i);
        printf("%02x ", text_data[i]);
    }
    printf("\n\n");

    // Read symbols
    int num_syms = symtab_hdr->sh_size / sizeof(ElfW(Sym));
    ElfW(Sym) *symbols = malloc(symtab_hdr->sh_size);
    if (!symbols) {
        perror("unable to allocate memory for symbols");
        exit(EXIT_FAILURE);
    }
    fseek(file, symtab_hdr->sh_offset, SEEK_SET);
    fread(symbols, sizeof(ElfW(Sym)), num_syms, file);

    // Read symbol string table
    char *strtab = malloc(strtab_hdr->sh_size);
    if (!strtab) {
        perror("unable to allocate memory for strtab");
        exit(EXIT_FAILURE);
    }
    fseek(file, strtab_hdr->sh_offset, SEEK_SET);
    fread(strtab, strtab_hdr->sh_size, 1, file);

    // Dump functions in .text
    printf("Functions in .text:\n");
    int text_section_index = (int)(text_section - section_headers);

    for (int i = 0; i < num_syms; i++) {
        if (ELF_ST_TYPE(symbols[i].st_info) == STT_FUNC &&
            symbols[i].st_shndx == text_section_index &&
            symbols[i].st_size > 0) {

            const char *func_name = strtab + symbols[i].st_name;
            size_t offset = symbols[i].st_value - text_section->sh_addr;
            size_t size = symbols[i].st_size;

            printf("\nFunction: %s (size: %zu bytes)\n", func_name, size);
            for (size_t j = 0; j < size && offset + j < text_section->sh_size; j++) {
                if (j % 16 == 0) printf("\n%08zx  ", j);
                printf("%02x ", text_data[offset + j]);
            }
            printf("\n");
        }
    }

    // Cleanup
    free(symbols);
    free(strtab);
    free(shstrtab);
    free(text_data);
    free(section_headers);
    fclose(file);
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
