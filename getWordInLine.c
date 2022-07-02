#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getWordInLine(char *fileName, int lineNumber, char *p) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int i=1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (i==lineNumber) {
           printf("%s", line);
           strcpy(p, line);
           return;
        }
        i++;
    }
}

int main(int arg, char* argv[]) {
     //Chequeos de args y stat del archivo
     char palabra[6];//El tamanio es 6 por llevar el caracter de final de string
     int pos = atoi(argv[2]);
     getWordInLine(argv[1], pos, palabra);

     printf("La palabra en la ubicacion %d es %s\n", pos, palabra);

     return 0;
}
