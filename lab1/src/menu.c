#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct functions_description {  
    char *name;
    char (*fun)(char);
};

char* map(char *array, int array_length, char (*f) (char));


/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
    return fgetc(stdin); 
}

/* If c is a number between 0x20 and 0x7E,
 cprt prints the character of ASCII value c followed by a new line.
 Otherwise, cprt prints the dot ('.') character.
  After printing, cprt returns the value of c unchanged. */
char cprt(char c){
  if (c >= 0x20 && c <= 0x7E) 
      printf("%c\n", c);
  else printf(".\n");
  return c;
}

/* Gets a char c. If c is between 0x20 and 0x4E add 0x20 to its value and return it. Otherwise return c unchanged */
char encrypt(char c) {
    if (c >= 0x20 && c <= 0x4E)
        return c + 0x20;
    
    else return c;
}


/* Gets a char c and returns its decrypted form subtractng 0x20
 from its value. But if c was not between 0x40
  and 0x7E it is returned unchanged */
char decrypt(char c) {
    if (c >= 0x40 && c <= 0x7E)
        return c - 0x20;
     else return c;
}


/* xoprt prints the value of c in a hexadecimal representation, then in octal representation, followed by a new line, and returns c unchanged. */
char xoprt(char c){
    printf("%x ", c);// Print hexadecimal representation
    printf("%o\n", c);// Print octal representation

    return c;
}

int main(int argc, char **argv) {
    struct functions_description menu[] = { {"Get string", &my_get} , {"Print string" , &cprt}, {"Encrypt", &encrypt} , {"Decrypt", &decrypt}, {"Print Hex and Octal", &xoprt}, {NULL, NULL}};
    char carray[5] = "";
    char option[3] = "";
    while (true) {
        // Read an input line from stdin
        printf("Select operation from the following menu (ctrl^D for exit):\n");
        int i = 0;

        // print all the items from the menu
        while(menu[i].name != NULL)
        {
            printf("%d) %s\n",i, menu[i].name);
            i++;
        }
        printf("Option: ");

        if(fgets(option, 3, stdin)==NULL)
            break;

        if(option[0] >= '0' && option[0] <= '4' && (option[1] == '\n' || option[1]=='\0'))
        {
            printf("Within bounds\n");
            char* result = map(carray, 5, menu[option[0] - '0'].fun);
            strcpy(carray, result); // Copy the result into carray
            free(result); // Free the allocated memory
            printf("Done.\n\n");
        }
        else {
            if(option[0]!= EOF)
                printf("%s Not within bounds\n", option);
            break;
        }
    }
    return 0;
}

char* map(char *array, int array_length, char (*f) (char)) {
    char* mapped_array = (char*)(malloc((array_length + 1) * sizeof(char))); // +1 for null terminator
    if (mapped_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    else{
        // Apply the function to each character in the input array
        for (int i = 0; i < array_length; i++) {
            mapped_array[i] = (*f)(array[i]);
        }
        mapped_array[array_length] = '\0'; // Null-terminate the mapped array
        return mapped_array;
    }
}


