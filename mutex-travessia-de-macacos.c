#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 // Macacos que andam de A para B
#define MB 10 // Macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER; // Controla o acesso a corda, macacos na mesma direcao ou gorila
pthread_mutex_t lock_turno = PTHREAD_MUTEX_INITIALIZER; // Garante que macacos de direcoes opostas nao entrem na corda ao mesmo tempo
pthread_mutex_t lock_macacosAB = PTHREAD_MUTEX_INITIALIZER; // Protege a variavel macacos_na_corda
pthread_mutex_t lock_macacosBA = PTHREAD_MUTEX_INITIALIZER;

int macacosAB = 0;
int macacosBA = 0;

void *macacoAB(void *arg);
void *macacoBA(void *arg);
void *gorila(void *arg);

int main () {
    pthread_t macacos[MA + MB];
    int *id;
    int i = 0;
    for (i = 0; i < MA + MB; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        if (i % 2 == 0) {
            if (pthread_create(&macacos[i], NULL, macacoAB, (void *) id)) {
                printf("Erro na criacao da thread do macaco %d\n", *id);
                return -1;
            }
        } else {
            if (pthread_create(&macacos[i], NULL, macacoBA, (void *) id)) {
                printf("Erro na criacao da thread do macaco %d\n", *id);
                return -1;
            }
        }
    }
    pthread_t g;
    if (pthread_create(&g, NULL, gorila, NULL)) {
        printf("Erro na criacao da thread do gorila\n");
        return -1;
    }
    pthread_join(macacos[0], NULL);
}

void *macacoAB(void *arg) {
    int id = *((int *) arg);
    while (1) {
        pthread_mutex_lock(&lock_turno); // Tem macaco indo de A para B nesse momento
        pthread_mutex_lock(&lock_macacosAB); // Protege a variavel macacosAB
            macacosAB++;            
            if (macacosAB == 1) {
                pthread_mutex_lock(&lock_corda); // O primeiro macaco AB indo locka a corda
            }
        pthread_mutex_unlock(&lock_macacosAB);                             
        pthread_mutex_unlock(&lock_turno);                

        printf("Macaco %d indo de A para B. Macacos indo de A para B: %d\n", id, macacosAB);                    
        sleep(1);
        pthread_mutex_lock(&lock_macacosAB);                   
            macacosAB--;                      
            if (macacosAB == 0) {
                pthread_mutex_unlock(&lock_corda);
            }                                    
        pthread_mutex_unlock(&lock_macacosAB);

        printf("Macaco %d chegou em B. Macacos indo de A para B: %d\n", id, macacosAB);
    }
    pthread_exit(0);
}

void *macacoBA(void *arg) {
    int id = *((int *) arg);
    while (1) {
        pthread_mutex_lock(&lock_turno);
        pthread_mutex_lock(&lock_macacosBA);
            macacosBA++;
            if (macacosBA == 1) {
                pthread_mutex_lock(&lock_corda);
            }
        pthread_mutex_unlock(&lock_macacosBA);                
        pthread_mutex_unlock(&lock_turno);
        
        printf("Macaco %d indo de B para A. Macacos indo de B para A: %d\n", id, macacosBA);
        sleep(1);
        pthread_mutex_lock(&lock_macacosBA);
            macacosBA--;            
            if (macacosBA == 0) {            
                pthread_mutex_unlock(&lock_corda);                
            }                        
        pthread_mutex_unlock(&lock_macacosBA);
        
        printf("Macaco %d chegou em A. Macacos indo de B para A: %d\n", id, macacosBA);    
        
    }
    pthread_exit(0);
}

void *gorila(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock_turno);
        pthread_mutex_lock(&lock_corda);
        
        printf("Gorila atravessando a corda.\n");                                      
        pthread_mutex_unlock(&lock_turno);
        sleep(rand() % 2 + 2);        
        
        printf("Gorila chegou do outro lado.\n");
        
        pthread_mutex_unlock(&lock_corda);
        
    }
    pthread_exit(0);
}
