#include <stdio.h>
#include <stdlib.h>
int main(){
  int count = 0;
  FILE *fp;
  char file_char;
  fp = fopen("palabras.txt", "r");
  
  while((file_char = (char)fgetc(fp)) != EOF ){
    if(file_char == '\n') count++;
  }
  printf("%d",count);
  return 0;
}
