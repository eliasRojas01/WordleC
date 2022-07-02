#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>//la importe para strcpy y strchr
#include <unistd.h>//la importe para pid

#define WORD_LENGTH 6 // 5chars + '\0'
#define MAX_GAMES 8
#define MIN_GAMES 1
#define TRIES_PER_GAME 6
#define START_SCORE 5000
#define ONE_SHOT_WIN 10000
#define PENALTY 500

void getWordInLine(char*, int, char*);//funcion provista 
void set_feedback(char[],char[],char[],int );
int set_score(char[], int[], int);
void reset_tracking(int[]);

int main(){
  char misterious_word[WORD_LENGTH], guess[WORD_LENGTH];//en misterious_word se almacenara la palabra a adivinar y en guess el intento
  char words_used[MAX_GAMES][WORD_LENGTH];
  char feedback_board[WORD_LENGTH*TRIES_PER_GAME];//En este array unidimensional se tendra un registro de los intentos en cada partida, el + 6 es para incluir el caracter '\n' que separara cada registro
  char wanna_play, keep_playing;//quiere jugar o no? quiere seguir jugando?
  int games;//cantidad de partidas
  int scores[MAX_GAMES] = {START_SCORE};//Este array tendra la puntuacion de cada partida y sera previamente utilizado para mostrar el mayor y menor puntaje de cada juego y el promedio de los puntajes
  int tracking[WORD_LENGTH-1] = {0};//cada numero representara la posicion de cada letra de una palabra, al final de la partida se seteara todo a 0 de nuevo
  int i, j;

  //cambiar la seed en cada proceso
  srand(getpid());

  printf("Queres participar de una sesion de juego?: \n (S)i/(N)o \n");
  do{
    printf(">");
    scanf(" %c", &wanna_play);
  }while(!(wanna_play == 'S' || wanna_play=='N'));//si wanna_play es igual a N o a S, salgo del loop

  if( wanna_play == 'N' ){//Si el usuario ingreso N, el programa termina
    printf("Bye bye\n");
    return 0;
  }

  //Preguntar cuantas partidas desea jugar el usuario
  printf("Ingrese la cantidad de partidas(max. 8)\n");
  do{
    printf("\ncantidad de partidas: ");
    scanf("%d",&games);
  }while(games > MAX_GAMES || games < MIN_GAMES);


  for(i = 0; i < games; i++){//i sera el numero de partida ;
    getWordInLine("palabras.txt",rand()%30+1, misterious_word);// obtengo una palabra "random" del archivo palabras.txt 

    strcpy(words_used[i], misterious_word);//copia en el registro de palabras utilizadas, la ultima palabra a adivinar

    printf("------Partida %d de %d------\n", i+1, games);
    for(j = 0 ; j < TRIES_PER_GAME; j++){//j seran los intentos por partida, j de jugada (?)

      printf("Adivine la palabra: ");
      scanf("%s", guess);

      set_feedback(feedback_board,misterious_word,guess,j);//set_feedback comparara las strings e ira completando el array feedback_board
      scores[i] += set_score(feedback_board,tracking,j);//set_score obtendra la puntuacion de cada jugada y ademas se le pasara el tracking

      if(scores[i] > START_SCORE){
	printf("Excelente! Gano la partida \n");
	break;
      }else{
	print_feedback(feedback_board, j);
	printf("%s", guess);
      }
    }
    reset_tracking(tracking);

    printf("desea seguir jugando? (S)i/ (N)o\n");
    scanf(" %c", &keep_playing);
    if(keep_playing == 'N') break;
  }

  return 0;
}

void reset_tracking(int A[]){
  int i;
  for(i = 0; i < WORD_LENGTH-1; i++){
    A[i]=0;
  }
}

void getWordInLine(char *fileName, int lineNumber, char *p) {//funcion provista 
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

void set_feedback(char feedback_board[],char word[],char guess[],int try_num){
  int i;
  //comparo los arrays para completar el feedback_board
  for(i = 0; i < WORD_LENGTH; i++ ){
    if(guess[i] == word[i]){//si los primeros caracteres estan en la misma posicion, agrega un +
      feedback_board[WORD_LENGTH*try_num+i] = '+';
    }else if(strchr(word,guess[i]) != NULL){//si el caracter esta en la cadena, no devolvera un puntero nulo y colocara * en la posicion
      feedback_board[WORD_LENGTH*try_num+i] = '*';
    }else{
      feedback_board[WORD_LENGTH*try_num+i] = '-';
    }
  }

  feedback_board[WORD_LENGTH*(try_num+1)-1] = '\n';//agregar un caracter de nueva linea despues de cada palabra (cada 5 11 17... indices)
}

int set_score(char feedback_board[], int tracking[], int try_num){
  int try_score = 0, i,j;

  for(i = try_num*WORD_LENGTH ,j=0; feedback_board[i] != '\n' ; i++ , j++){
    if(feedback_board[i] == '+' && tracking[j]==0){
      tracking[j]= 100;
      try_score += 100;
    }else if(feedback_board[i] == '*' && tracking[j]==0){
      tracking[j]= 50;
      try_score += 50;
    }
  }

  if(try_score == 500){
    return ONE_SHOT_WIN;//si en el primer intento acierta, devuelve ONE_SHOT_WIN
  }else{
    return try_score-PENALTY;// si no, devolvera los puntos obtenidos - una penalidad por no adivinar la palabra
  }
}
