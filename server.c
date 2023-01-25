#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Database not fount. usage: %s <db name>\n", argv[0]);
        return 1;
    }
}