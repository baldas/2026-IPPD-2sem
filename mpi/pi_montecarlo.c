#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>     /* For MPI functions, etc */ 

int main(int argc, char* argv[]) {

  unsigned long i, count, n;
  double x,y,z,pi;
  struct timeval start, stop;
  int        comm_sz;               /* Number of processes    */
  int        my_rank;               /* My process rank        */
   
  MPI_Init(&argc, &argv); 

  if (argc < 2) {
    printf("Necessário informar um número N.\n");
    exit(-1);
  }
  n = strtoll(argv[1], NULL, 10);
   
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
  
  srand(my_rank);

  gettimeofday(&start, NULL);
  
  if (my_rank == 0) {   // master
    unsigned long trabalho = n/comm_sz;
    for (int q = 1; q < comm_sz; q++) 
      MPI_Send(&trabalho, 1, MPI_UNSIGNED_LONG, q, 0, MPI_COMM_WORLD); 
    n = trabalho;
  }
  else
    MPI_Recv(&n, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  
  count = 0;

  for (i=0; i < n; ++i) {

    x = (double)rand() / RAND_MAX;
    y = (double)rand() / RAND_MAX;

    z = x * x + y * y;

    if( z <= 1 ) count++;
  }
  
  if (my_rank == 0) {   // master
    // Coleta as somas locais
    long soma_local = 0;
    long total = 0;
    for (int q = 1; q < comm_sz; q++) {
      MPI_Recv(&soma_local, 1, MPI_UNSIGNED_LONG, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
      total += soma_local;
    }
    total += count; // adiciona valor computador pela master
  
    pi = ((double) total / n * 4) / comm_sz;
  
    gettimeofday(&stop, NULL);

    printf(" Nossa estimativa de pi = %.14f\n", pi);
    printf("                     pi = %.14f\n", 4.0*atan(1.0));
   
    double t = (((double)stop.tv_sec)*1000.0 + ((double)(stop.tv_usec)/1000.0)) - \
             (((double)start.tv_sec)*1000.0 + ((double)(start.tv_usec)/1000.0));

    fprintf(stdout, "Tempo gasto: %g ms\n", t);
  }
  else
    MPI_Send(&count, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  return(0);
}
