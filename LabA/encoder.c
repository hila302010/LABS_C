#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void encode(char *encKey, FILE* inFile, FILE* outFile);
int getEncChar(int charInput, int keyDigit);
int getDecChar(int charInput, int keyDigit);


int main(int argc, char **argv) {
    // initialize the variables 
    char* encKey = NULL;
    int debugMode = 1;
    FILE * inFile=stdin;
    FILE * outFile=stdout;

    // first read the arguments in the command line
    for(int i = 1; i < argc; i++)
    {
      switch (argv[i][0]) // check the first charcter of the current argument
      {
        case '-': // check arguments that start with -
          if (argv[i][1] == 'D') 
            debugMode = 0;
          else if (argv[i][1] == 'e')
            encKey = argv[i];
          else if(argv[i][1] == 'I')
            inFile = fopen(argv[i]+2,"r");
          else if(argv[i][1] == 'O')
            outFile= fopen(argv[i]+2,"w");
          break;

        case '+': // check arguments that start with +
          if (argv[i][1] == 'D') 
            debugMode = 1;
          else if (argv[i][1] == 'e')
            encKey = argv[i];
          break;
        default:
          break;
      }
      if(debugMode == 1)
          fprintf(stderr, "%s\n", argv[i]);
    }

    if(encKey!=NULL)  //should encode
      encode(encKey, inFile, outFile);

    else{ // no need to encode, get input until detecting EOF
      int c=0;
      while((c = fgetc(inFile)) != EOF)
        fputc(c, outFile);
    }


    // close the files 
    fclose(inFile);
    fclose(outFile);


    return 0;
}

void  encode(char * encKey, FILE* inFile, FILE* outFile){
    int charInput=0, charOutput=0; // int ascii value of chars for input and output
    int keyIndex = 2; // Start after the '+e' or '-e'
    int keyDigit; // the digit of the encryption
    char keyChar; // char of the key

    while((charInput = fgetc(inFile)) != EOF){

      // Get the key digit for this character
      keyChar = encKey[keyIndex++];
      if (keyChar == 0) { // 0 is NULL terminator of strings
        keyIndex = 2; // Reset to beginning of key
        keyChar = encKey[keyIndex++];
      }
      keyDigit = keyChar - '0'; // Convert char to int

      switch (encKey[0])
      {
        case '+':
          charOutput = getEncChar(charInput, keyDigit);
          fputc(charOutput, outFile);
          break;

        case '-':
          charOutput = getDecChar(charInput, keyDigit);
          fputc(charOutput, outFile);
          break;
      
        default:
          break;
      }   
    }
}

int getEncChar(int charInput, int keyDigit)
{
  int charOutput = 0;
  // small letter
  if(charInput >= 97 && charInput <= 122)
      charOutput = 97 + ((charInput + keyDigit) - 97) % 26; 
  // number
  else if(charInput >= 48 && charInput <= 57)
      charOutput = 48 + ((charInput + keyDigit) - 48) % 10; 
  else 
    charOutput = charInput;

  return charOutput; 
}

int getDecChar(int charInput, int keyDigit)
{
  int charOutput = 0;
  // small letter
  if(charInput >= 97 && charInput <= 122)
      charOutput = 97 + ((charInput - keyDigit) + 26 - 97) % 26;
  //number 
  else if(charInput >= 48 && charInput <= 57)
      charOutput = 48 + ((charInput - keyDigit) + 10 - 48) % 10; 
  else 
    charOutput = charInput;

  return charOutput;

}