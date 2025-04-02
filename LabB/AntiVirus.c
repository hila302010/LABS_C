#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 3
#define BUFFER_SIZE 10240 // 10K bytes

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

struct functions_description {
    char *name;
};
typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

void SetSigFileName();
void printVirus(virus* v, FILE* file);
virus* readVirus(FILE* file);
void checkMagicNumber(FILE* file);
unsigned short convertBEtoHost(unsigned short value);
void loadSignatures(link **virus_list);

void list_print(link *virus_list, FILE *file);
link* list_append(link* virus_list, virus* data);
void list_free(link *virus_list);

void detect_virus(char *buffer, unsigned int size, link *virus_list);
void detectViruses(link *virus_list, const char *filename);
void neutralize_virus(char *fileName, int signatureOffset);
void fixFile(char *fileName, link *virusList);
void quit(link *virus_list);

typedef struct {
    int offset;
    virus *v;
} VirusLocation;

VirusLocation detectedViruses[100];
int detectedVirusCount = 0;

struct functions_description menuItems[] = {
    {"Set signatures file name"},
    {"Load signatures"},
    {"Print signatures"},
    {"Detect viruses"},
    {"Fix file"},
    {"Quit"},
    {NULL}
};

int endian; // 0 means big endian, 1 means little endian
char currSigFileName[256] = "signatures-L";

/*
decription:
input:
output:
*/
void SetSigFileName() {
    printf("Enter new signature file name: ");
    fgets(currSigFileName, 256, stdin);
    // Remove newline character
    currSigFileName[strcspn(currSigFileName, "\n")] = 0;
}

void printVirus(virus* v, FILE *file) {
    fprintf(file, "Virus name: %s\n", v->virusName);
    fprintf(file, "Virus signature length: %u\n", v->SigSize);
    fprintf(file, "Virus signature:\n");
    for (int i = 0; i < v->SigSize; i++) {
        fprintf(file, "%02X ", v->sig[i]);
    }
    fprintf(file, "\n\n");
}

unsigned short convertBEtoHost(unsigned short value) {
    return (value >> 8) | (value << 8);
}

virus* readVirus(FILE* file) {
    virus* v = (virus*)malloc(sizeof(virus));
    if (v == NULL) {
        perror("Failed to allocate memory for virus");
        exit(1);
    }

    if (fread(v, 1, 18, file) != 18) {
        free(v);
        return NULL;
    }
    if (endian == 0) {
        v->SigSize = convertBEtoHost(v->SigSize);
    }

    v->sig = (unsigned char*)malloc(v->SigSize);
    if (v->sig == NULL) {
        perror("Failed to allocate memory for virus signature");
        free(v);
        exit(1);
    }

    if (fread(v->sig, 1, v->SigSize, file) != v->SigSize) {
        free(v->sig);
        free(v);
        return NULL;
    }

    return v;
}

void loadSignatures(link ** virus_list) {
    FILE *sigFile = fopen(currSigFileName, "rb");
    if (!sigFile) {
        perror("Error opening file");
        exit(1);
    }
    checkMagicNumber(sigFile);
    virus* v;
    while ((v = readVirus(sigFile)) != NULL) {
        *virus_list = list_append(*virus_list, v);
    }
    fclose(sigFile);
    printf("Signatures loaded successfully.\n");
}

void checkMagicNumber(FILE* file) {
    char magicNumber[4];
    if (fread(magicNumber, 1, 4, file) != 4) {
        fprintf(stderr, "Error reading magic number.\n");
        exit(1);
    }

    if (strncmp(magicNumber, "VIRL", 4) == 0) {
        endian = 1; // Little-endian
    } else if (strncmp(magicNumber, "VIRB", 4) == 0) {
        endian = 0; // Big-endian
    } else {
        fprintf(stderr, "Invalid magic number.\n");
        exit(1);
    }

    //printf("Endian: %s\n", endian == 0 ? "Big-endian" : "Little-endian");
}

void list_print(link* virus_list, FILE* file) {
    link *current = virus_list;
    while (current != NULL) {
        printVirus(current->vir, file);
        current = current->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data) {
    link* newLink = (link*)malloc(sizeof(link));
    if (newLink == NULL) {
        perror("Failed to allocate memory for link");
        exit(1);
    }
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

void list_free(link *virus_list) {
    link *current = virus_list;
    while (current != NULL) {
        link *next = current->nextVirus;
        free(current->vir->sig);
        free(current->vir);
        free(current);
        current = next;
    }
}

void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    for (unsigned int i = 0; i < size; i++) 
    {
        link *current = virus_list;
        while (current != NULL) 
        {
            virus *v = current->vir;
            if (memcmp(buffer + i, v->sig, v->SigSize) == 0) {
                printf("\nVirus detected!\nStarting byte location: %u\nVirus name: %s\nVirus signature size: %u\n\n",
                    i, v->virusName, v->SigSize);
                detectedViruses[detectedVirusCount].offset = i;
                detectedViruses[detectedVirusCount].v = v;
                detectedVirusCount++;
            }
            current = current->nextVirus;
        }
    }
    
}

void detectViruses(link *virus_list, const char *filename) {
    FILE *suspectedFile = fopen(filename, "rb");
    if (!suspectedFile) {
        perror("Error opening suspected file");
        return;
    }

    char buffer[BUFFER_SIZE];
    unsigned int fileSize = fread(buffer, 1, BUFFER_SIZE, suspectedFile);
    fclose(suspectedFile);

    detect_virus(buffer, fileSize, virus_list);
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, signatureOffset, SEEK_SET);
    unsigned char retInstruction = 0xC3;
    fwrite(&retInstruction, 1, 1, file);

    fclose(file);
}


void fixFile(char *fileName, link *virusList) {
    for (int i = 0; i < detectedVirusCount; i++) {
        neutralize_virus(fileName, detectedViruses[i].offset);
    }
    printf("All detected viruses have been neutralized.\n");
}

void quit(link *virus_list) {
    list_free(virus_list);
    printf("Exiting program.\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    //FILE* outputFile; should we print to output file or screen?
    link *virusList = NULL;

    if (argc == 2) {
        strncpy(currSigFileName, argv[1], sizeof(currSigFileName) - 1);
    }
    char option[MAX_LINE] = "";
    
    while (1) {
        printf("Select operation from the following menu:\n");
        int i = 0;
        while(menuItems[i].name != NULL) {
            printf("%d) %s\n", i, menuItems[i].name);
            i++;
        }
        printf("Option: ");

        if(fgets(option, MAX_LINE, stdin) == NULL)
            break;

        int intOption = atoi(option);

        switch (intOption) {
            case 0:
                SetSigFileName();
                break;
            case 1:
                loadSignatures(&virusList);
                break;
            case 2:
                list_print(virusList, stdout);
                break;
            case 3:
                if (argc == 3) {
                    detectViruses(virusList, argv[2]);
                } else {
                    printf("Please provide the suspected file as a command-line argument.\n");
                }
                break;
            case 4:
                fixFile(argv[2], virusList);
                break;
            case 5:
                quit(virusList);
                break;
            default:
                printf("Wrong option! try again\n");
                break;
        }
    }
}
