#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 10000

long int M[SIZE][SIZE];
long int v1[SIZE], v2[SIZE];

int thread_count;

void *thread_mv(void *arg)
{
  long i, j;

  long my_id = (long)arg;
  long tbloco = SIZE / thread_count;

  long i_inicial = tbloco*my_id;
  long i_final = i_inicial+tbloco;

  if (my_id == thread_count-1) i_final = SIZE;

  for (i = i_inicial; i<i_final; i++) {
    v2[i] = 0;
    for(j=0; j<SIZE; j++) {
      v2[i] += M[i][j]*v1[j];
    }
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  struct timeval start, stop;

  long i,j;
  for (i=0; i<SIZE; i++) {
    v1[i] = i;
    for (j=0; j<SIZE; j++)
      M[i][j] = i+j;
  }

  pthread_t *thread_handles;
 
  thread_count = strtol(argv[1], NULL, 10);
  thread_handles = malloc(thread_count*sizeof(pthread_t));

  gettimeofday(&start, NULL);

  /* FORK Cria as threads informadas na linha de comando */
  for (i=0; i<thread_count; i++) {
    if (pthread_create(&thread_handles[i], NULL, thread_mv, (void *)i) != 0) {
      fprintf(stderr, "Nao consegui criar a thread\n");
      exit(-1);
    }
  }

  /* JOIN */
  for (i=0; i<thread_count; i++)
  {
    pthread_join(thread_handles[i], NULL);
  }

  free(thread_handles);
  
  // JOIN
  
  gettimeofday(&stop, NULL);


  double temp = \
    (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
    ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));

  fprintf(stdout, "Time elapsed = %g ms\n", temp);

  for (i=0; i<SIZE; i++)
    fprintf(stdout, "v[%d]=%ld\n", i, v2[i]);

	return 0;
}

