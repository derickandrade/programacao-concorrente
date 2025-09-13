#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3
#define NL 10

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER; // Lock para o recurso compartilhado
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER; // Não deixa bagunçar o número de leitores
pthread_mutex_t lock_turno = PTHREAD_MUTEX_INITIALIZER;

int num_leitores = 0;

void * reader(void * arg);
void * writer(void * arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {
    pthread_t r[NL], w[NE];
    int i;
    int *id;

    // Cria leitores
    for (i = 0; i < NL; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&r[i], NULL, reader, (void *) (id));
    }

    // Cria escritores
    for (i = 0; i < NE; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&w[i], NULL, writer, (void *) (id));
    }
    pthread_join(r[0], NULL);
    return 0;
}

void * reader(void * arg) {
    int i = *((int *) arg);
    while(TRUE) {
        pthread_mutex_lock(&lock_turno); // Se o escritor tiver passado o turno, o escritor fica esperando aqui
        pthread_mutex_lock(&lock_nl); // Os leitores podem começar 
        
            num_leitores++;
            if (num_leitores == 1) {
                pthread_mutex_lock(&lock_bd); // Primeiro leitor bloqueia o escritor
            }
        pthread_mutex_unlock(&lock_nl);
        pthread_mutex_unlock(&lock_turno); // Libera o próximo turno, seja leitor ou escritor

        read_data_base(i); // Região não crítica

        pthread_mutex_lock(&lock_nl);
            num_leitores--;
            if (num_leitores == 0) {
                pthread_mutex_unlock(&lock_bd); // Último leitor libera o escritor
            }
        pthread_mutex_unlock(&lock_nl);

        use_data_read(i); // Região não crítica
    }
}

void * writer(void * arg) {
    int i = *((int *) arg);
    while(TRUE) {
        think_up_data(i); // Região não crítica
        pthread_mutex_lock(&lock_turno); // Pega o turno pra escrever
        pthread_mutex_lock(&lock_bd); // Pega o BD pra escrever
        pthread_mutex_unlock(&lock_turno); // Libera o próximo turno
            write_data_base(i); // Região crítica
        pthread_mutex_unlock(&lock_bd); // Mas só libera o BD depois de escrever
    }
    pthread_exit(0);
}

void read_data_base(int i) {
    printf("Leitor %d: lendo dados. Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5);
}
void use_data_read(int i) {
    printf("Leitor %d está usando os dados lidos. Número de leitores: %d\n", i, num_leitores );
    sleep(rand() % 5);
}

void think_up_data(int i) {
    printf("Escritor %d está pensando nos dados a serem escritos.\n", i);
    sleep(rand() % 5);
}

void write_data_base(int i) {
    printf("Escritor %d: escrevendo dados. Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 2 + 4);
}
