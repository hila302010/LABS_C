#include <stdio.h>
#include <ctype.h>

int count_digits(const char *str) {
    int count = 0;
    while (*str) {
        if (isdigit(*str)) {
            count++;
        }
        str++;
    }
    return count;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    int digit_count = count_digits(argv[1]);
    printf("String contains %d digits, right???\n", digit_count);

    return 0;
}
