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
#define WIN_SCORE 2000
#define ONE_SHOT_WIN 10000
#define PENALTY 500

void getWordInLine(char*, int, char*);//funcion provista 
int set_feedback(char[], char[]);
void reset_tracking(char[]);
void print_feedback(char[],char[], char[]);
int user_won(char[]);

void print_mean(int[],int);
void print_higher_score(int[],int);
void print_lower_score(int[],int);

int main(){
  char misterious_word[WORD_LENGTH], guess[WORD_LENGTH], guess_feedback[WORD_LENGTH-1];//en misterious_word se almacenara la palabra a adivinar y en guess el intento
  char wanna_play, keep_playing;//quiere jugar o no? quiere seguir jugando?
  int total_games;//cantidad de partidas
  int scores[MAX_GAMES];//Este array tendra la puntuacion de cada partida
  char tracking[WORD_LENGTH-1] = {'-'};//cada numero representara la posicion de cada letra de una palabra, al final de la partida se seteara todo a 0 de nuevo
  int i, j;

  //cambiar la seed en cada proceso
  srand(getpid());

  printf("Bienvenido a WORDLE\n");
  printf("Este programa no repite letras\n");
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
    printf(">");
    scanf("%d",&total_games);
  }while(total_games > MAX_GAMES || total_games < MIN_GAMES);

  for(i = 0; i < total_games; i++){//i sera el numero de partida ;
    getWordInLine("palabras.txt",rand()%30+1, misterious_word);// obtengo una palabra "random" del archivo palabras.txt 
    scores[i] = START_SCORE;//cada partida comienza con 5000 

    printf("------Partida %d de %d------\n", i+1, total_games);
    for(j = 0 ; j < TRIES_PER_GAME; j++){//j seran los intentos por partida, j de jugada (?)

      scanf("%s", guess);

      print_feedback(guess_feedback,misterious_word,guess);//printea un feedback y ademas guess_feedback
      scores[i] += set_feedback(tracking, guess_feedback); //set_feedback obtendra la puntuacion de cada jugada y ademas se le pasara el tracking

      if(user_won(guess_feedback) && j==0){
	printf("Excelente! Ganaste a la primera!\n");
	scores[i]=ONE_SHOT_WIN;
	break;//sale de la partida
      }else if(user_won(guess_feedback)){
	printf("Ganaste!\n");
	break;//sale de la partida
      }
    }
    if(j == 6)scores[i] = 0;//si no termino la partida antes, deja el puntaje de la partida en 0
    printf("La palabra era %s\n",misterious_word);
    printf("Puntaje obtenido : %d\n",scores[i]);
    reset_tracking(tracking);

    printf("Desea seguir jugando? (S)i/(N)o\n");
    scanf(" %c", &keep_playing);
    if(keep_playing == 'N') break;
  }

  print_mean(scores, total_games);
  print_higher_score(scores, total_games);
  print_lower_score(scores, total_games);

  return 0;
}

void reset_tracking(char A[]){
  int i;
  for(i = 0; i < WORD_LENGTH-1; i++){
    A[i]='+';
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
      //printf("%s", line);
      strcpy(p, line);
      return;
    }
    i++;
  }
}

int set_feedback(char tracking[],char guess_feedback[]){
  int try_score = 0, i;

  for(i = 0; i < WORD_LENGTH; i++){
    if(guess_feedback[i] == '+' && tracking[i] != '.'){//si la pego y es nueva, suma 100
      tracking[i] = '.';
      try_score += 100;
    }else if(guess_feedback[i] == '*' && tracking[i] != '.'){//si casi la pega y es nueva, suma 50
      tracking[i] = '.';
      try_score += 50;
    }
  }
  if(user_won(guess_feedback)){
    return WIN_SCORE;
  }else{
    return try_score-PENALTY;
  }
}
void print_feedback(char feedback[],char word[], char guess[]){
  int i = 0;
  while(i < WORD_LENGTH-1){
    if( word[i] == guess[i] ){
      feedback[i] = '+';
      printf("+");
    }else if(strchr(word, guess[i]) != NULL){
      feedback[i] = '-';
      printf("*");
    }else{
      feedback[i] = '-';
      printf("-");
    }
    i++;
  }
  printf("\n");
}

int user_won(char guess_feedback[]){//determina si el usuario gano si detecta una seguidilla de + en el guess_feedback
  int i, count = 0;
  for(i = 0; i < WORD_LENGTH-1; i++){
    if(guess_feedback[i] == '+'){
      count++;
    }
  }
  if(count >= 5) return 1;
  return 0;
}

void print_mean(int scores[], int games){
  int i;
  float total;
  for(i = 0; i < games; i++){
    total += scores[i];
  }
  printf("El puntaje promedio de las partidas fue %.2f\n",total);
}
void print_higher_score(int scores[],int games){
  int i, greater_num = 0;
  for(i = 0; i < games; i++){
    if(scores[i] > greater_num){
      greater_num=scores[i];
    }
  }
  printf("El puntaje mas alto del juego fue %d en la partida %d\n",greater_num,i);
}
void print_lower_score(int scores[],int games){
  int i, lowest_num = ONE_SHOT_WIN;
  for(i = 0; i < games; i++){
    if(scores[i] < lowest_num){
      lowest_num=scores[i];
    }
  }
  printf("El puntaje mas bajo del juego fue %d en la partida %d\n",lowest_num,i);
}
