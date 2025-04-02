#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

const char* get_phdr_type(Elf32_Word p_type) {
    switch (p_type) {
        case PT_NULL: return "NULL";
        case PT_LOAD: return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP: return "INTERP";
        case PT_NOTE: return "NOTE";
        case PT_SHLIB: return "SHLIB";
        case PT_PHDR: return "PHDR";
        case PT_TLS: return "TLS";
        default: return "UNKNOWN";
    }
}

void print_phdr_info(Elf32_Phdr *phdr, int index) {
    printf("%-8s ", get_phdr_type(phdr->p_type));
    printf("0x%06x ", phdr->p_offset);
    printf("0x%08x ", phdr->p_vaddr);
    printf("0x%08x ", phdr->p_paddr);
    printf("0x%05x ", phdr->p_filesz);
    printf("0x%05x ", phdr->p_memsz);

    printf("%c", (phdr->p_flags & PF_R) ? 'R' : ' ');
    printf("%c", (phdr->p_flags & PF_W) ? 'W' : ' ');
    printf("%c ", (phdr->p_flags & PF_X) ? 'E' : ' ');

    printf("0x%x\n", phdr->p_align);

    // Compute mmap protection flags
    int prot_flags = 0;
    if (phdr->p_flags & PF_R) prot_flags |= PROT_READ;
    if (phdr->p_flags & PF_W) prot_flags |= PROT_WRITE;
    if (phdr->p_flags & PF_X) prot_flags |= PROT_EXEC;

    printf("PROT: %d ", prot_flags);

    int map_flags = MAP_PRIVATE | MAP_FIXED;
    printf("MAP: %d\n", map_flags);
}

void load_phdr(Elf32_Phdr *phdr, int fd) {
    if (phdr->p_type != PT_LOAD) {
        return;
    }

    // Compute mmap protection flags
    int prot_flags = 0;
    if (phdr->p_flags & PF_R) prot_flags |= PROT_READ;
    if (phdr->p_flags & PF_W) prot_flags |= PROT_WRITE;
    if (phdr->p_flags & PF_X) prot_flags |= PROT_EXEC;

    // Align the virtual address
    void *aligned_vaddr = (void *)((phdr->p_vaddr / 0x1000) * 0x1000);
    size_t offset_in_page = phdr->p_vaddr % 0x1000;

    // Adjust sizes to be page aligned
    size_t aligned_filesz = phdr->p_filesz + offset_in_page;
    size_t aligned_memsz = phdr->p_memsz + offset_in_page;

    int map_flags = MAP_PRIVATE | MAP_FIXED;

    // Debug output before mapping
    printf("Mapping segment: offset=0x%06x, vaddr=0x%08x, filesz=0x%05x, memsz=0x%05x, prot=%d, flags=%d\n",
           phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz, prot_flags, map_flags);

    // Map the segment into memory
    void *addr = mmap(aligned_vaddr, aligned_memsz, prot_flags, map_flags, fd, phdr->p_offset - offset_in_page);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Print the program header information
    print_phdr_info(phdr, 0);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;

    // Verify ELF magic number
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file\n");
        return -1;
    }

    // Iterate over program headers
    Elf32_Phdr *phdr = (Elf32_Phdr *)((char *)map_start + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; i++) {
        func(&phdr[i], arg);
    }

    return 0;
}

extern int startup(int argc, char **argv, void (*start)());

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <executable> [args...]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Get the file size
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    // Map the file into memory
    void *map_start = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Print header
    printf("Type     Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n");

    // Iterate over the program headers
    foreach_phdr(map_start, load_phdr, fd);

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;

    // Call the startup function
    int ret = startup(argc-1, argv+1, (void *)(ehdr->e_entry));

    // Unmap the file from memory
    munmap(map_start, file_size);

    // Close the file descriptor
    close(fd);

    return ret;
}
