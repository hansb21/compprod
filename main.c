#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define SIZE 2

int* buffer;

sem_t *cheio, *vazio, *lock_prod, *lock_cons;

int f = 0;
int i = 0;

void* producer(void* args) {
	
	while (1) {
	
		sem_wait(vazio);
		sem_wait(lock_prod);
		
		f = (f+1) % SIZE;
		buffer[f] = 1;

		printf("produzi no indice %d ID: %d\n", f, (int)args);	
		
		sem_post(lock_prod);
		sem_post(cheio);	
		//sleep(1);
	}
	pthread_exit(NULL);
}

void* consumer(void* args) {
	
	while (1) {
		sem_wait(cheio);
		sem_wait(lock_cons);
		
		i = (i+1)  % SIZE;
		buffer[i] = 0;
		
		printf("consumi no indice %d ID: %d\n",  i, (int)args);
		
		sem_post(lock_cons);
		sem_post(vazio);
		

		sleep(1);
	}
	pthread_exit(NULL);
}

int main(int argc, char const* argv[]) { 

	if (argc != 3) {
		printf("Usage: ./main <nprod> <ncons>");
		return 1;
	}

	buffer  = malloc(sizeof(int) * SIZE);
	cheio = malloc(sizeof(sem_t)); 
	vazio = malloc(sizeof(sem_t));
	lock_prod = malloc(sizeof(sem_t));
	lock_cons = malloc(sizeof(sem_t));

	
	sem_init(cheio, 0, 0);
	sem_init(vazio, 0, SIZE);
	sem_init(lock_prod, 0, 1);
	sem_init(lock_cons, 0, 1);

	int  prod = atoi(argv[1]);
	int cons = atoi(argv[2]);

	pthread_t threads[cons+prod];
	
	int n = 0;
	
	for (; n < cons; ++n) {
		pthread_create(&threads[n], NULL, consumer, n);
	}

	for (; n < cons + prod; ++n) {
		pthread_create(&threads[n], NULL, producer, n-cons);
	}
	for (int i = 0; i < cons+prod; ++i) {
		pthread_join(threads[i], NULL);

	}		
	sem_destroy(cheio);	
	sem_destroy(vazio);
	sem_destroy(lock_prod);
	sem_destroy(lock_cons);

	free(cheio);
	free(vazio);
	free(lock_prod);
	free(lock_cons);
	}

