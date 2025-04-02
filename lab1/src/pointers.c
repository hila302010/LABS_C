#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int Z = 0;

int *pointer_to_Z(int x) {
    /* function returning integer pointer, not pointer to function */
    return &Z;
}

float get_Z(int x, float y) {
    return x+y;
}

int main(int argc, char **argv)
{
    float (*function_pointer_to_Z)(int, float); // pointer to function taking an int as argument and returning an int
    function_pointer_to_Z = &get_Z;

    printf("pointer_to_Z output: %d\n", *pointer_to_Z(3));
    printf("pointer_to_Z output: %p\n", pointer_to_Z(3));
    printf("function_pointer_to_Z output: %f\n", (*function_pointer_to_Z)(3,2.0));

}
  
