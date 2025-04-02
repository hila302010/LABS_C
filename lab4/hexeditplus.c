#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INPUT_MAX_LEN 128

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char display_mode; // 0 for decimal, 1 for hexadecimal
} state;

void toggle_debug_mode(state *s);
void set_file_name(state *s);
void set_unit_size(state *s);
void load_into_memory(state *s);
void toggle_display_mode(state *s);
void memory_display(state *s);
void save_into_file(state *s);
void memory_modify(state *s);
void quit(state *s);

struct fun_desc {
    char *name;
    void (*fun)(state *s);
};

struct fun_desc menu[] = {
    {"Toggle Debug Mode", toggle_debug_mode},
    {"Set File Name", set_file_name},
    {"Set Unit Size", set_unit_size},
    {"Load Into Memory", load_into_memory},
    {"Toggle Display Mode", toggle_display_mode},
    {"Memory Display", memory_display},
    {"Save Into File", save_into_file},
    {"Memory Modify", memory_modify},
    {"Quit", quit},
    {NULL, NULL}
};

void print_debug_info(state *s) {
    if (s->debug_mode) {
        fprintf(stderr, "\nDebug: unit_size=%d, file_name='%s', mem_count=%zu\n\n", s->unit_size, s->file_name, s->mem_count);
    }
}

void toggle_debug_mode(state *s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag now %s\n", s->debug_mode ? "on" : "off");
}

void set_file_name(state *s) {
    printf("Enter file name: ");
    scanf("%127s", s->file_name);
    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void set_unit_size(state *s) {
    int size;
    printf("Enter unit size (1, 2, or 4): ");
    scanf("%d", &size);
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", size);
        }
    } else {
        printf("Invalid unit size\n");
    }
}

void load_into_memory(state *s) {
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE *file = fopen(s->file_name, "rb");
    if (!file) {
        printf("Error: cannot open file '%s'\n", s->file_name);
        return;
    }

    char input[100];
    unsigned int location;
    int length;

    printf("Please enter <location> <length>\n");
    fgets(input, sizeof(input), stdin); // Consume the newline left by previous input

    if (sscanf(input, "%x %d", &location, &length) != 2) {
        printf("Error: invalid input\n");
        fclose(file);
        return;
    }

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file_name='%s', location=0x%x, length=%d\n", s->file_name, location, length);
    }

    fseek(file, location, SEEK_SET);
    size_t bytes_read = fread(s->mem_buf, s->unit_size, length, file);
    s->mem_count = bytes_read * s->unit_size;

    printf("Loaded %zu units into memory\n", bytes_read);

    fclose(file);
}

void toggle_display_mode(state *s) {
    s->display_mode = !s->display_mode;
    if (s->display_mode) {
        printf("Display flag now on, hexadecimal representation\n");
    } else {
        printf("Display flag now off, decimal representation\n");
    }
}

void memory_display(state *s) {
    char input[100];
    unsigned int addr;
    int u;

    printf("Please enter <addr> <u>\n");
    fgets(input, sizeof(input), stdin); // Consume the newline left by previous input

    if (sscanf(input, "%x %d", &addr, &u) != 2) {
        printf("Error: invalid input\n");
        return;
    }

    unsigned char *start_addr = (addr == 0) ? s->mem_buf : (unsigned char *)(intptr_t)addr;

    static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

    for (int i = 0; i < u; i++) {
        int val;
        memcpy(&val, start_addr + i * s->unit_size, s->unit_size);
        if (s->display_mode) {
            printf(hex_formats[s->unit_size - 1], val);
        } else {
            printf(dec_formats[s->unit_size - 1], val);
        }
    }
}

void save_into_file(state *s) {
    unsigned char *source = NULL;
    int target;
    int length;
    char input[1024];

    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%p %x %d", &source, &target, &length) == 3) {
        if (source == NULL) {
            source = s->mem_buf;
        }
        
        // Open file with O_CREAT to ensure it exists, and O_WRONLY for write access. Specify mode 0666.
        int file = open(s->file_name, O_WRONLY | O_CREAT, 0666);
        if (file < 0) {
            perror("Failed to open file");
            return;
        }

        // Check if target position is beyond current file size
        off_t size = lseek(file, 0, SEEK_END);
        if (size < 0 || target > size) {
            fprintf(stderr, "Target position is invalid\n");
            close(file);
            return;
        }

        if (lseek(file, target, SEEK_SET) < 0) {
            perror("Failed to seek to target position");
            close(file);
            return;
        }

        size_t written = write(file, source, length * s->unit_size);
        if (written < 0) {
            perror("Failed to write to file");
        } else if (written < length * s->unit_size) {
            fprintf(stderr, "Partial write. Expected to write %d bytes, but wrote %zd bytes.\n", length * s->unit_size, written);
        }

        close(file);
    } else {
        fprintf(stderr, "Invalid input format.\n");
    }
}


void memory_modify(state* s) {
    // Prompt the user for location and val (all in hexadecimal).
    printf("Please enter <location> <val>\n");
    char input[INPUT_MAX_LEN] = {0};
    fgets(input,INPUT_MAX_LEN,stdin);
    int location;
    int val;
    if (sscanf(input, "%X %X\n", &location, &val)) {
        // If debug mode is on, print the location and val given by the user.
        if (s->debug_mode) {
            fprintf(stderr, "Debug: location: %X\n", location);
            fprintf(stderr, "Debug: val: %X\n", val);
        }
        // Replace a unit at location in the memory with the value given by val.
        memcpy(s->mem_buf + location, &val, 4); // sizeof(val) == 4
    } else {
        fprintf(stderr, "Invalid input given, expected <location> and <val> in hexadecimal.\n");
        return;
    }
}



void quit(state *s) {
    if (s->debug_mode) {
        printf("quitting\n");
    }
    exit(0);
}

int main(int argc, char **argv) {
    state s = {0, "", 1, {0}, 0};
    int choice;
    while (1) {
        print_debug_info(&s);
        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }
        printf("Option: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline left by scanf
        if (choice >= 0 && choice < sizeof(menu) / sizeof(menu[0]) - 1) {
            menu[choice].fun(&s);
        } else {
            printf("Invalid option\n");
        }
    }
    return 0;
}