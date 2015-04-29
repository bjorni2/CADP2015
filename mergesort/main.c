#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

void mergesort(int *arr, int lower, int upper);
void* tmergesort(void* vargp);
void merge(int *arr, int lower, int middle, int upper);
void sort(int *arr, size_t N);

struct ms_pars{
	int* arr;
	int lower;
	int upper;
};

sem_t threads;

int main(int argc, char** argv)
{
	sem_init(&threads, 0, 190);
	int SZ = atoi(argv[1]);
	clock_t begin, end;
	double time_spent;
	int* arr = malloc(sizeof(int) * SZ);
	int i;

	for(i = 0; i < SZ; i++){
		arr[i] = rand();
	}

	printf("Calling sort\n");
	begin = clock();
	sort(arr, SZ);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Returned from sort after %f\n", time_spent);

	return 0;
}

void posix_error(int code, char *msg) /* Posix-style error */
{
	printf("%s: %s\n", msg, strerror(code));
	exit(0);
}

void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
			void * (*routine)(void *), void *argp) 
{
	int rc;

	if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0){
		posix_error(rc, "Pthread_create error");
	}
}

void Pthread_join(pthread_t tid, void **thread_return) {
	int rc;

	if ((rc = pthread_join(tid, thread_return)) != 0)
	posix_error(rc, "Pthread_join error");
}

void mergesort(int *arr, int lower, int upper){
	int middle = (lower + upper) / 2;
	if(lower < upper){
		sem_trywait(&threads);
		if(errno == EAGAIN)
		{
			mergesort(arr, lower, middle);
			mergesort(arr, middle+1, upper);
			merge(arr, lower, middle, upper);
		}
		else{
			struct ms_pars* par = malloc(sizeof(struct ms_pars));
			par->arr = arr;
			par->lower = middle+1;
			par->upper = upper;

			pthread_t tid;
			int rc;
			if((rc = pthread_create(&tid, NULL, tmergesort, par)) != 0){
				printf("pthread_create error: %d\n", rc);
			}

			mergesort(arr, lower, middle);
			//mergesort(arr, middle+1, upper);

			Pthread_join(tid, NULL);
			sem_post(&threads);
			free(par);
			
			merge(arr, lower, middle, upper);
		}
	}
}

void* tmergesort(void* vargp){
	struct ms_pars* pin = (struct ms_pars*)vargp;
	int* arr = pin->arr;
	int lower = pin->lower;
	int upper = pin->upper;

	int middle = (lower + upper) / 2;
	if(lower < upper){
		sem_trywait(&threads);
		if(errno == EAGAIN)
		{
			mergesort(arr, lower, middle);
			mergesort(arr, middle+1, upper);
			merge(arr, lower, middle, upper);
		}
		else{
			struct ms_pars* par = malloc(sizeof(struct ms_pars));
			par->arr = arr;
			par->lower = middle+1;
			par->upper = upper;

			pthread_t tid;
			Pthread_create(&tid, NULL, tmergesort, par);

			mergesort(arr, lower, middle);

			Pthread_join(tid, NULL);
			sem_post(&threads);
			free(par);
			
			merge(arr, lower, middle, upper);
		}
	}
}

void merge(int *arr, int lower, int middle, int upper){
	int size = (upper - lower) + 1;
	int* tmp = malloc(sizeof(int) * (size));

	int left = lower;
	int right = middle + 1;

	int i = 0;

	while(i < size){
		if(left <= middle && ((right <= upper && arr[left] < arr[right]) || right > upper)){
			tmp[i] = arr[left];
			i++;
			left++;
		}
		if(right <= upper && ((left <= middle && arr[right] <= arr[left]) || left > middle)){
			tmp[i] = arr[right];
			i++;
			right++;
		}
	}

	i = 0;
	while(i < size){
		arr[i+lower] = tmp[i];
		i++;
	}

	free(tmp);
}

void sort(int *arr, size_t N){
	mergesort(arr, 0, N-1);
}
