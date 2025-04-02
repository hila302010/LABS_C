#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

/*int main(int argc, char **argv) {
  int base_len = 5;
  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get);
  char* arr3 = map(arr2, base_len, cprt);
  char* arr4 = map(arr3, base_len, xoprt);
  char* arr5 = map(arr4, base_len, encrypt);
  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
}*/