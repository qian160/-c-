#include<stdio.h>
#include<elf.h>
#include<string.h>
#include<stdbool.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/mman.h>
/*
        steps:
            1. Read the ELF header, use its 'shstrndx' and 'shoff' field to find the section headers table and .shstrtab section(holds strings for all sections' name)
            2. Tranverse the section headers table, compare the type and name field and find .strtab and .symtab(start of the elf file pluses the offset)
                (to compare the section's name, the shstrtab will be needed)
            3. To print the symbols, use the data type Elf64_Sym to tranverse the .symtab
        some notes:
            1. The field 'st_name' and 'sh_name' is not a 'name' in fact, to get the string name, we need to use this name field to index into string table
            2. section headers are not sections, they can not directly be used. To find the true sections, use the offset field in the section header and jump there
*/
static void load_elf(char * elf_file) {
    if(elf_file == NULL)
        return;
    else
    {
        int fd = open(elf_file, O_RDONLY);
        if(fd < 0){
                printf("failed to open the file\n");
                return;
        }
        struct stat sb;
        if(fstat(fd, &sb) == -1){
            printf("fstat error\n");
            exit(0);
        }
        char * elf_file = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(elp_file == MAP_FAILED){
           printf("map failed\n");
           exit(0);
        }
        close(fd);
        Elf64_Ehdr *elf_header = (Elf64_Ehdr *)elf_file;
        unsigned char *id = elf_header->e_ident;

        if(id[0] != 0x7f || id[1] != 'E' || id[2] != 'L' || id[3] != 'F'){
                printf("bad elf file\n");
                munmap(elf_file,sb.st_size);
                exit(0);
        }

        Elf64_Half shnum = elf_header -> e_shnum;
        Elf64_Half shstrndx = elf_header -> e_shstrndx;
        Elf64_Off shoff = elf_header -> e_shoff;

        Elf64_Shdr * shdr = (Elf64_Shdr *)(elf_file + shoff);
        char * shstrtab = elf_file + (shdr + shstrndx)->sh_offset;
        char * strtab;
        Elf64_Sym * symtab;

        //find the strtab and symtab
        int i = 0;
        bool find1 = false, find2 = false;
        Elf64_Word symbol_num;
        Elf64_Shdr *this;
        for(this = shdr; i < shnum; this++){
            if(find1 && find2)break;
            Elf64_Off offset = this->sh_offset;
            char * name = shstrtab + this-> sh_name;
            if(!find1 && this->sh_type == SHT_SYMTAB){
                symtab = (Elf64_Sym *)(elf_file + offset);
                //printf("find .symtab at 0x%lx\n", offset);
                symbol_num = this -> sh_size / sizeof(Elf64_Sym);
                find1 = true;
            }
            else if(!find2 && strcmp(".strtab", name) == 0){
                strtab = elf_file + offset;
                find2 = true;
                //printf("find .strtab at 0x%lx\n", offset);
            }
            i++;
        }
        Elf64_Xword sh_size = shnum * sizeof(Elf64_Shdr);
        i = 0;
        printf("Section Headers:\n");
        printf(" Num: \t\t\t\t Value \t Size \t TYpe \n");
        for(this = shdr; i < shnum; this++){
            char * name = shstrtab + this->sh_name;
            printf("%2d: %-20s \t %3lx \t %lx \t %x \n", i++, name, this->sh_size, this->sh_offset, this->sh_name);
        }

        printf("\nSymbol Table '.symtab' contains %d entries:\n", symbol_num);
        i = 0;
        printf(" Num: \t Value  \t Size  \t Type  \t Bind  \t Vis  \t Ndx  \t    Name\n");
        for(Elf64_Sym * sym = symtab; i < symbol_num; sym++){
            unsigned char info = sym -> st_info;
            unsigned char type = ELF64_ST_TYPE(info);
            unsigned char bind = ELF64_ST_BIND(info);
            unsigned char vis  = ELF64_ST_VISIBILITY(info);
            char * name = strtab + sym->st_name;
            printf("%2d: %8lx %8ld %8d %8d %8d %8d %16s\n", i++, sym->st_value, sym->st_size, type, bind, vis, sym->st_shndx, name);
        }
        munmap(elf_file, sb.st_size);
    }

}
int main(int argc, char **argv)
{
    if(argc < 2){
        printf("too few arguments\n");
        return 0;
    }
    load_elf(argv[1]);
}
