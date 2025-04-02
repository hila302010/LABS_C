#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();
void infection();
void infector(char*);

int main (int argc , char* argv[], char* envp[])
{
  char* c = "VIRUS ATTACHED\n";
  if (argc < 2) {
    system_call(1, 0x55, 0, 0); // Exit with error code 0x55
    return 0;
    }

  // Check if the first argument is "-a" without using strcmp
  if (argv[1][0] == '-' && argv[1][1] == 'a') {
    char* filename = &argv[1][2]; // Extract filename starting right after "-a"
    infection();
    system_call(SYS_WRITE, STDOUT, c, 15); // Print "VIRUS ATTACHED"
    infector(filename);
    }
    return 0;
}