#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>


double f(double x){
  return sin(x);
}

int main(int argc, char** argv)
{

  //variable initalization
  int comm_sz, my_rank;
  double local_sum=0.0, global_sum=0.0, intervals; 
  double global_start=0.0, global_end=M_PI, local_start=0.0, local_end=0.0;
  int integrationType;


  //ensure proper command line format
  if(argc == 3)
  {
    integrationType = atoi(argv[1]);
    intervals = atof(argv[2]);
  }
  else
  {
    printf("correct ussage ./mpi {integration type} {intervals}\n");
  }
  

  double starttime, endtime;

  //mpi init and get info about system
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  //start timing
  starttime = MPI_Wtime();
  
  //calculate this ranks working area
  local_start = my_rank * (intervals/comm_sz);
  local_end = (my_rank+1) * (intervals/comm_sz);
  
  local_sum = 0.0;

  //calculate step size
  double step_size = (global_end-global_start)/intervals;


  //preform integration type based on command line input flag
  if(integrationType == 1)
  {
    for(int i = local_start; i < local_end; i++)
    {

      //calculate endpoint
      double right = global_start + (i*step_size +step_size);
      //calculate area
      double local = f(right) * step_size;
      local_sum += (local);
    }
  }
  else if(integrationType == 2)
  {

    for(int i = local_start; i < local_end; i++)
    {
      //calculate endpoint
      double left = global_start + (i*step_size);
      //calculate area
      double local = f(left) * step_size;
      local_sum += (local);
    }
  }
  else if(integrationType == 3)
  {
     for(int i = local_start; i < local_end; i++)
    {
      //calculate endpoints
      double left = global_start + (i*step_size);
      double right = global_start + (i*step_size + step_size);
      //calculate area
      double local = (( f(left) + f(right) ) / 2 ) * step_size;
      local_sum += (local);
    }
  }
  else if(integrationType == 4)
  { 
    int flip;
    float stepThree = step_size/3.0;
    if(local_start==0){
      local_sum = f(0);
      local_start = 1;
    }
    else{
      local_sum = 0;
    }

    for(int i = local_start; i < local_end; i ++)
    {
      if(i == intervals-1){
        double temp = local_start + (intervals*step_size);
        double local = f(temp);
        local_sum+=local;
      }
      else{
        flip = (2 * pow(2,i%2));
        double temp = local_start + (i*step_size);
        double local = f(temp) * flip;
        local_sum += local;
      }
    }

    local_sum = stepThree * local_sum;
  }

  //gather sum from all nodes
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  //stop timing
  endtime   = MPI_Wtime();

  //rank 0 prints all information
  if(my_rank == 0)
  {
    if(integrationType == 1)
    {
      printf("Method: Right Reimans Sum\n");
    }
    else if(integrationType == 2)
    {
      printf("Method: Left Reimans Sum\n");
    }
    else if(integrationType == 3)
    {
      printf("Method: Trapeziodal\n");
    }
    else if(integrationType == 4)
    {
      printf("Method: Simpsons\n");
    }
    printf("Sum: %.20f\n", global_sum);
    printf("Error: %.20f\n", 2.0-global_sum);
    printf("Time: %f\n",endtime-starttime);
  }

  MPI_Finalize();

  return 0;
}