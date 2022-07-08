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
#define WELL_PLACED_LETTER 100
#define WRONG_PLACED_LETTER 50

void getWordInLine(char*, int, char*);//funcion provista 
int set_score(char[], char[]);
void reset_tracking(char[]);
void print_feedback(char[],char[], char[]);
void clean_stdin();
void capitalize_word(char[]);
int user_won(char[]);
void print_mean(int[],int);
void print_higher_score(int[],int);
void print_lower_score(int[],int);

int main(){
  char misterious_word[WORD_LENGTH], guess[WORD_LENGTH], try_feedback[WORD_LENGTH-1];//en misterious_word se almacenara la palabra a adivinar ,en guess el intento y try_feedback tendra el feedback de cada intento
  char wanna_play, keep_playing;//quiere jugar o no? quiere seguir jugando?
  int total_games;//cantidad de partidas
  int scores[MAX_GAMES];//Este array tendra la puntuacion de cada partida donde cada partida esta representada por el numero de la partida, menos uno.
  char tracking[WORD_LENGTH-1] = {'-'};//cada numero representara la posicion de cada letra de la palabra de una partida,al acertar/adivinar un caracter se completara con '.' al final de la partida se seteara todo a '-' de nuevo
  int i, j;

  //cambiar la seed en cada proceso
  srand(getpid());

  printf("Bienvenido a WORDLE\n");
  printf("Este programa no repite letras y ademas, solo tendra en cuenta las 5 primeras letras de un intento\n");

  printf("Quiere participar de una sesion de juego?: \n (S)i/(N)o \n");
  do{
    printf(">");
    scanf(" %c", &wanna_play);
  }while(!(wanna_play == 'S' || wanna_play=='N'));//si wanna_play es igual a N o a S, salgo del loop

  if( wanna_play == 'N' ){//Si el usuario ingreso N, el programa termina
    printf("Bye bye\n");
    return 0;
  }

  printf("Ingrese la cantidad de partidas(max. 8): \n");
  do{
    printf(">");
    scanf("%d",&total_games);
  }while(total_games > MAX_GAMES || total_games < MIN_GAMES);

  for(i = 0; i < total_games; i++){//i sera el numero de partida ;
    getWordInLine("palabras.txt",rand()%30+1, misterious_word);// obtengo una palabra "random" del archivo palabras.txt 
    scores[i] = START_SCORE;//cada partida comienza con 5000 

    printf("------Partida %d de %d------\n", i+1, total_games);
    for(j = 0 ; j < TRIES_PER_GAME; j++){//j seran los intentos por partida
      scanf("%5s", guess);//Se leeran solo los primeros 5 char
      clean_stdin();// un buffer es un area temporal de almacenamiento,todos los dispositivos tienen buffers de entrada y salida, cuando entra mas informacion de la necesaria, esta queda guardada en el buffer de entrada, entonces ni bien se vuelva a pedir input, esta informacion "basura" sera tomada por la proxima funcion. Por esto uso clean_stdin(),encontrada en github por cierto, para que el proximo loop no agarre "basura" del anterior.
      capitalize_word(guess);//Se pasaran todas las letras del intento a mayusculas si no las tienen

      print_feedback(try_feedback,misterious_word,guess);//printea un feedback y ademas try_feedback obtiene la ultima jugada
      scores[i] += set_score(tracking, try_feedback); //set_score obtendra la puntuacion de cada jugada y ademas se le pasara el tracking para checkear si el usuario gano o no con user_won

      if(user_won(try_feedback) && j==0){
        printf("Excelente! Ganaste a la primera!\n");
        scores[i]=ONE_SHOT_WIN;
        break;//sale de la partida
      }else if(user_won(try_feedback)){
        printf("Ganaste!\n");
        break;//sale de la partida
      }
    }
    if(j == 6)scores[i] = 0;//si no termino la partida antes, deja el puntaje de la partida en 0
    printf("La palabra era /%s/",misterious_word);
    printf("Puntaje de la partida : %d\n",scores[i]);
    reset_tracking(tracking);

    if(total_games > 1){ //si tiene mas de un juego, consultar para seguir jugando
      printf("Desea seguir jugando? (S)i/(N)o\n");
      scanf(" %c", &keep_playing);
    }
    if(keep_playing == 'N')i++;break;
  }
  printf("-----ESTADISTICAS-----\n");
  print_mean(scores, i);
  print_higher_score(scores, i);
  print_lower_score(scores, i);// el i+1 es porque la cantidad de juegos jugados(i) tiene un juego menos

  return EXIT_SUCCESS;
}

void reset_tracking(char A[]){
  int i;
  for(i = 0; i < WORD_LENGTH-1; i++){
    A[i]='-';
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

int set_score(char tracking[],char try_feedback[]){//recibira tracking para ver que letras fueron adivinadas ya, si la letra fue adivinada, no contara en el puntaje porque se marco previamente en tracking. Tambien recibira el intento de la jugada actual.
  int try_score = 0, i;

  for(i = 0; i < WORD_LENGTH; i++){
    if(try_feedback[i] == '+' && tracking[i] != '.'){//si la pega y es nueva, suma 100 y es marcada en tracking
      tracking[i] = '.';
      try_score += WELL_PLACED_LETTER;
    }else if(try_feedback[i] == '*' && tracking[i] != '.'){//si casi la pega y es nueva, suma 50 y es marcada en tracking
      tracking[i] = '.';
      try_score += WRONG_PLACED_LETTER;
    }
  }

  if(user_won(try_feedback)){
    return WIN_SCORE;
  }else{
    return try_score-PENALTY;
  }
}

void print_feedback(char feedback[],char word[], char guess[]){
  int i = 0;
  while(i < WORD_LENGTH-1){//loopeo por toda la string de manera que:
    if( word[i] == guess[i] ){//si encuentro caracteres iguales en la misma posicion de la palabra a adivinar y el intento, printeo + y completo feedback[i]
      feedback[i] = '+';
      printf("+");
    }else if(strchr(word, guess[i]) != NULL){//si encuentro que el caracter i del intento, se encuentra en la palabra a adivinar, printeo * y completo feedback[i]
      feedback[i] = '*';
      printf("*");
    }else{// si definitivamente el caracter no esta en la palabra a adivinar, printeo '-' y lo pongo en la misma posicion
      feedback[i] = '-';
      printf("-");
    }
    i++;
  }
  printf("\n");
}

int user_won(char try_feedback[]){//si detecta una seguidilla de 5 '+' en el intento actual, el usuario gana y devuelve 1, si no devuelve 0
  int i, count = 0;
  for(i = 0; i < WORD_LENGTH-1; i++){
    if(try_feedback[i] == '+'){
      count++;
    }
  }
  if(count >= 5) return 1;
  return 0;
}

void print_mean(int scores[], int games){//printea el promedio
  int i;
  float total = 0;
  for(i = 0; i < games; i++){
    total += scores[i];
  }
  printf("El puntaje promedio de las partidas fue %.3f\n",total/games);
}
void print_higher_score(int scores[],int games){
  int i, greater_num = 0;
  for(i = 0; i < games; i++){
    if(scores[i] > greater_num){
      greater_num=scores[i];
    }
  }
  printf("El puntaje mas alto del juego fue %d en la partida %d\n",greater_num,i-1);
}
void print_lower_score(int scores[],int games){
  int i, lowest_num;
  int game = 1; // guarda en que juego se obtuvo el menor puntaje, inizializado en 1 porque si no se llegara a encontrar algun puntaje mayor que el primero, tendria basura dentro
  for(i = 0; i < games; i++){
    if(scores[i] < lowest_num ){
      lowest_num=scores[i];
      game = i+1;
    }
  }
  printf("El puntaje mas bajo del juego fue %d en la partida %d\n",lowest_num,game);
}
void capitalize_word(char guess[]){
  //aprovechando de que las mayusculas y las minusculas tiene 32 posiciones de diferencia en la tabla ascii
  int i, ascii_character_num;
  char num_to_char;
  for(i = 0; i < WORD_LENGTH-1; i++){
    if(( ascii_character_num = guess[i] ) > 96){//si encuentro que el valor de numero del caracter en ascii es mayor que 96, definitivamente es una letra minuscula, entonces la paso a mayuscula restandole 32
      num_to_char = ascii_character_num-32;
      guess[i] = num_to_char;
    }
  }
}
void clean_stdin(void){
  int c;
  do{
    c = getchar();
  }while(c != '\n' && c != EOF);//mientras c sea distinto de \n y EOF, sigue tomando caracteres
}
