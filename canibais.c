#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_canibal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_canibal = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_cozinheiro = PTHREAD_COND_INITIALIZER;

int d = 0; // comida disponível
int a = 0;

void *canibal(void*meuid);
void *cozinheiro(int m);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
    //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram
    pthread_mutex_lock(&mutex);
    if (d == 0) {
      if (a == 0) {
        pthread_mutex_lock(&mutex_canibal);
        a = 1;
        pthread_mutex_unlock(&mutex_canibal);
        printf("%d: nao ha comida, vou acordar o cozinheiro\n", *(int *)(pi));
        pthread_cond_signal(&cond_cozinheiro);
      }      
      pthread_cond_wait(&cond_canibal, &mutex);
    }
      if (d > 0) {
        pthread_mutex_lock(&mutex_canibal);
        d--;
        printf("%d: peguei uma porcao, porcoes disponiveis = %d\n", *(int *)(pi), d);
        pthread_mutex_unlock(&mutex_canibal);
        if (d == 0) {
          printf("%d: vou acordar o cozinheiro\n", *(int *)(pi));
          pthread_cond_signal(&cond_cozinheiro);
        }
        pthread_mutex_unlock(&mutex);
    }
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(2);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
    pthread_mutex_lock(&mutex);
    while (d > 0) {
      //esperar até que as porções acabem
      printf("cozinheiro: esperando os canibais comerem\n");
      pthread_cond_wait(&cond_cozinheiro, &mutex);
    }
    printf("cozinheiro: vou cozinhar\n");
    sleep(5);
    d = m;
    printf("cozinheiro: terminei de cozinhar, porcoes disponiveis = %d\n", d);
    pthread_mutex_lock(&mutex_canibal);
    a = 0;
    pthread_mutex_unlock(&mutex_canibal);
    pthread_cond_broadcast(&cond_canibal);
    pthread_mutex_unlock(&mutex);
   }
}
