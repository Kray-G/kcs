#include <stdio.h>
#include <string.h>

void main() {
    char a[10];
    strcpy(a, "abcdef");
    printf("%s\n", &a[1]);
}
