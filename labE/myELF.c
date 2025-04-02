#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>

#define MAX_ELF_FILES 2

typedef struct {
    int fd;
    void* map_start;
    struct stat fd_stat;
} ElfFile;

ElfFile elfFiles[MAX_ELF_FILES] = {{-1, NULL, {0}}, {-1, NULL, {0}}};
int debugMode = 0;
int fileInderx = 0;
char fileNames[MAX_ELF_FILES][1000];  // Array for file names

const char* sectionTypeNames[] = {
    "NULL", "PROGBITS", "SYMTAB", "STRTAB", "RELA", "HASH", "DYNAMIC",
    "NOTE", "NOBITS", "REL", "SHLIB", "DYNSYM", "INIT_ARRAY", "FINI_ARRAY",
    "PREINIT_ARRAY", "GROUP", "SYMTAB_SHNDX", "NUM"
};

void toggleDebugMode() {
    debugMode = !debugMode;
    printf("Debug mode now %s\n", debugMode ? "On\n" : "Off\n");
}

void examineElfFile() {
    printf("Enter ELF file name: ");
    if(fileInderx == 0) {
        scanf("%s", fileNames[0]);
        fileInderx = 1;
    } else {
        scanf("%s", fileNames[1]);
    }

    int i;
    for (i = 0; i < MAX_ELF_FILES; i++) {
        if (elfFiles[i].fd == -1) break;
    }

    if (i == MAX_ELF_FILES) {
        printf("Maximum number of ELF files already mapped.\n");
        return;
    }

    int fd = open(fileNames[i], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return;
    }

    if (fstat(fd, &elfFiles[i].fd_stat) != 0) {
        perror("Stat failed");
        close(fd);
        return;
    }

    elfFiles[i].map_start = mmap(0, elfFiles[i].fd_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (elfFiles[i].map_start == MAP_FAILED) {
        perror("Error mmapping the file");
        close(fd);
        return;
    }

    elfFiles[i].fd = fd;

    Elf32_Ehdr* header = (Elf32_Ehdr*) elfFiles[i].map_start;
    if (header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || header->e_ident[EI_MAG2] != ELFMAG2) {
        printf("Not an ELF file\n");
        munmap(elfFiles[i].map_start, elfFiles[i].fd_stat.st_size);
        close(fd);
        elfFiles[i].fd = -1;
        return;
    }

    printf("File %s:\n", fileNames[i]);
    printf("Magic: %c%c%c\n", header->e_ident[EI_MAG1], header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
    printf("Data encoding: %s\n", header->e_ident[EI_DATA] == ELFDATA2LSB ? "Little endian" : "Big endian");
    printf("Entry point: 0x%x\n", header->e_entry);
    printf("Section header table offset: %u\n", header->e_shoff);
    printf("Number of section header entries: %d\n", header->e_shnum);
    printf("Size of each section header entry: %d\n", header->e_shentsize);
    printf("Program header table offset: %u\n", header->e_phoff);
    printf("Number of program header entries: %d\n", header->e_phnum);
    printf("Size of each program header entry: %d\n\n", header->e_phentsize);
}

void printSectionNames() {
    for (int i = 0; i < MAX_ELF_FILES; i++) {
        if (elfFiles[i].fd == -1) continue; // Skip uninitialized files

        Elf32_Ehdr* header = (Elf32_Ehdr*) elfFiles[i].map_start;
        Elf32_Shdr* sectionHeaders = (Elf32_Shdr*)(elfFiles[i].map_start + header->e_shoff);
        Elf32_Shdr* sectionHeaderStrTab = &sectionHeaders[header->e_shstrndx];
        char* sectionHeaderStrTabData = (char*)(elfFiles[i].map_start + sectionHeaderStrTab->sh_offset);

        printf("File %s\n", fileNames[i]);

        if (debugMode) {
            printf("Section header string table index: %d\n", header->e_shstrndx);
        }

        for (int j = 0; j < header->e_shnum; j++) {
            Elf32_Shdr* shdr = &sectionHeaders[j];
            const char* sectionName = sectionHeaderStrTabData + shdr->sh_name;
            const char* sectionTypeName = (shdr->sh_type < sizeof(sectionTypeNames) / sizeof(sectionTypeNames[0])) 
                                         ? sectionTypeNames[shdr->sh_type]
                                         : "UNKNOWN";

            printf("[%d] %s 0x%x 0x%x %u %s\n", j, sectionName, shdr->sh_addr, shdr->sh_offset, shdr->sh_size, sectionTypeName);

            if (debugMode) {
                printf("  Index: %d, Address: 0x%x, Offset: 0x%x, Size: %u, Type: %d\n", j, shdr->sh_addr, shdr->sh_offset, shdr->sh_size, shdr->sh_type);
            }
        }
    }
}

void printSymbols() {
    for (int i = 0; i < MAX_ELF_FILES; i++) {
        if (elfFiles[i].fd == -1) continue; // Skip uninitialized files

        Elf32_Ehdr* header = (Elf32_Ehdr*) elfFiles[i].map_start;
        Elf32_Shdr* sectionHeaders = (Elf32_Shdr*)(elfFiles[i].map_start + header->e_shoff);
        Elf32_Shdr* sectionHeaderStrTab = &sectionHeaders[header->e_shstrndx];
        char* sectionHeaderStrTabData = (char*)(elfFiles[i].map_start + sectionHeaderStrTab->sh_offset);

        printf("File %s\n", fileNames[i]);

        // Find the symbol table and string table sections
        Elf32_Shdr* symtabSection = NULL;
        Elf32_Shdr* strtabSection = NULL;
        for (int j = 0; j < header->e_shnum; j++) {
            if (sectionHeaders[j].sh_type == SHT_SYMTAB) {
                symtabSection = &sectionHeaders[j];
            }
            if (sectionHeaders[j].sh_type == SHT_STRTAB && strcmp(sectionHeaderStrTabData + sectionHeaders[j].sh_name, ".strtab") == 0) {
                strtabSection = &sectionHeaders[j];
            }
        }

        if (!symtabSection) {
            printf("No symbol table section found in %s\n", fileNames[i]);
            continue;
        }

        if (!strtabSection) {
            printf("No string table section found in %s\n", fileNames[i]);
            continue;
        }

        Elf32_Sym* symbols = (Elf32_Sym*)(elfFiles[i].map_start + symtabSection->sh_offset);
        char* strtab = (char*)(elfFiles[i].map_start + strtabSection->sh_offset);
        int numSymbols = symtabSection->sh_size / sizeof(Elf32_Sym);

        if (debugMode) {
            printf("Symbol table size: %d bytes\n", symtabSection->sh_size);
            printf("Number of symbols: %d\n", numSymbols);
        }

        for (int j = 0; j < numSymbols; j++) {
            Elf32_Sym* sym = &symbols[j];
            const char* symbolName = strtab + sym->st_name;
            const char* sectionName = (sym->st_shndx < header->e_shnum) ? sectionHeaderStrTabData + sectionHeaders[sym->st_shndx].sh_name : "UNDEFINED";

            printf("[%d] 0x%x %d %s %s\n", j, sym->st_value, sym->st_shndx, symbolName, (sym->st_shndx == SHN_UNDEF) ? "UNDEFINED" : sectionName);

            if (debugMode) {
                printf("  Symbol: %s, Value: 0x%x, Section: %d\n", symbolName, sym->st_value, sym->st_shndx);
            }
        }
    }
}

void checkFilesForMerge() {
    if (elfFiles[0].fd == -1 || elfFiles[1].fd == -1) {
        printf("Both ELF files must be loaded for merging.\n");
        return;
    }

    // Check that each ELF file contains exactly one symbol table
    Elf32_Ehdr* header1 = (Elf32_Ehdr*) elfFiles[0].map_start;
    Elf32_Ehdr* header2 = (Elf32_Ehdr*) elfFiles[1].map_start;
    Elf32_Shdr* sectionHeaders1 = (Elf32_Shdr*)(elfFiles[0].map_start + header1->e_shoff);
    Elf32_Shdr* sectionHeaders2 = (Elf32_Shdr*)(elfFiles[1].map_start + header2->e_shoff);

    Elf32_Shdr* symtabSection1 = NULL;
    Elf32_Shdr* symtabSection2 = NULL;

    for (int i = 0; i < header1->e_shnum; i++) {
        if (sectionHeaders1[i].sh_type == SHT_SYMTAB) {
            if (symtabSection1 != NULL) {
                printf("Feature not supported: More than one symbol table in first ELF file.\n");
                return;
            }
            symtabSection1 = &sectionHeaders1[i];
        }
    }

    for (int i = 0; i < header2->e_shnum; i++) {
        if (sectionHeaders2[i].sh_type == SHT_SYMTAB) {
            if (symtabSection2 != NULL) {
                printf("Feature not supported: More than one symbol table in second ELF file.\n");
                return;
            }
            symtabSection2 = &sectionHeaders2[i];
        }
    }

    if (symtabSection1 == NULL || symtabSection2 == NULL) {
        printf("Feature not supported: Symbol table not found in one of the ELF files.\n");
        return;
    }

    Elf32_Sym* symbols1 = (Elf32_Sym*)(elfFiles[0].map_start + symtabSection1->sh_offset);
    Elf32_Sym* symbols2 = (Elf32_Sym*)(elfFiles[1].map_start + symtabSection2->sh_offset);
    char* strtab1 = (char*)(elfFiles[0].map_start + sectionHeaders1[symtabSection1->sh_link].sh_offset);
    char* strtab2 = (char*)(elfFiles[1].map_start + sectionHeaders2[symtabSection2->sh_link].sh_offset);

    int numSymbols1 = symtabSection1->sh_size / sizeof(Elf32_Sym);
    int numSymbols2 = symtabSection2->sh_size / sizeof(Elf32_Sym);

    for (int i = 1; i < numSymbols1; i++) {  // Skip the first symbol which is a dummy symbol
        Elf32_Sym* sym1 = &symbols1[i];
        const char* name1 = strtab1 + sym1->st_name;

        if (sym1->st_shndx == SHN_UNDEF) {
            int found = 0;
            for (int j = 1; j < numSymbols2; j++) {  // Skip the first symbol which is a dummy symbol
                Elf32_Sym* sym2 = &symbols2[j];
                const char* name2 = strtab2 + sym2->st_name;

                if (strcmp(name1, name2) == 0) {
                    if (sym2->st_shndx == SHN_UNDEF) {
                        printf("Symbol %s undefined\n", name1);
                    } else {
                        printf("Symbol %s multiply defined\n", name1);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Symbol %s undefined\n", name1);
            }
        } else {
            int found = 0;
            for (int j = 1; j < numSymbols2; j++) {  // Skip the first symbol which is a dummy symbol
                Elf32_Sym* sym2 = &symbols2[j];
                const char* name2 = strtab2 + sym2->st_name;

                if (strcmp(name1, name2) == 0) {
                    if (sym2->st_shndx != SHN_UNDEF) {
                        printf("Symbol %s multiply defined\n", name1);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Symbol %s undefined\n", name1);
            }
        }
    }
}

void mergeElfFiles() {
    if (elfFiles[0].fd == -1 || elfFiles[1].fd == -1) {
        printf("Both ELF files must be loaded for merging.\n");
        return;
    }

    // Call CheckMerge to perform the necessary checks
    checkFilesForMerge();
    printf("Merge ELF files functionality is not yet implemented.\n");
}

void quit() {
    for (int i = 0; i < MAX_ELF_FILES; i++) {
        if (elfFiles[i].fd != -1) {
            munmap(elfFiles[i].map_start, elfFiles[i].fd_stat.st_size);
            close(elfFiles[i].fd);
            elfFiles[i].fd = -1;
        }
    }
    exit(0);
}

typedef void (*menuFunction)();

typedef struct {
    char* name;
    menuFunction func;
} MenuOption;

MenuOption menu[] = {
    {"Toggle Debug Mode", toggleDebugMode},
    {"Examine ELF File", examineElfFile},
    {"Print Section Names", printSectionNames},
    {"Print Symbols", printSymbols},
    {"Check Files for Merge", checkFilesForMerge},
    {"Merge ELF Files", mergeElfFiles},
    {"Quit", quit},
    {NULL, NULL}
};

int main() {
    while (1) {
        printf("\nChoose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }

        int choice;
        scanf("%d", &choice);
        if (choice < 0 || choice >= sizeof(menu) / sizeof(menu[0]) - 1) {
            printf("Invalid option\n");
            continue;
        }

        menu[choice].func();
    }

    return 0;
}