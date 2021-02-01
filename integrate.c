#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>


//The function we will be integrating.
double f(double x){
  return sin(x);
}

//to calculate difference between two timespec structures
//code borrowed from stack overflow
struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(int argc, char const *argv[])
{	

	//int to hold command line input for integration type
	int integrationType;

	//variables needed to preform integration
	double start, end, stepSize, sum = 0.0;
	int i, intervals;

	//Check that program was run with correct ammount of arguments
	if(argc == 3)
	{
		//store command line arguments in variables
		integrationType = atoi(argv[1]);
		intervals = atof(argv[2]);
	}
	else
	{
		printf("correct ussage ./integrate {integration type} {intervals}\n");
	}

	/**********

	integrationType
	
	1 = right reimans sum
	2 = left reimans sum
	3 = trapazoidal sum
	4 = simpsons


	**********/

	//Declare variables for timing and begin timing.
	struct timespec start_clock, finish, total;
	clock_gettime(CLOCK_MONOTONIC, &start_clock);

	//integrate between 0 and pi for our functions
	start = 0;
	end = M_PI;

	//width of each rectangle in the integration
	stepSize = (end-start)/intervals;

	if(integrationType == 1)
	{
		printf("Method: Right Reimans Sum\n");
		for(i = 0; i < intervals; i ++)
		{
			//calculate endpoint
			double right = start + (i*stepSize + stepSize);
			//calculate area
			double local = f(right) * stepSize;
			sum += local;
		}
	}
	else if(integrationType == 2)
	{
		printf("Method: Left Reimans Sum\n");
		for(i = 0; i < intervals; i++)
		{
			//calculate endpoint
			double left = start + (i*stepSize);
			//calculate area
			double local = f(left) * stepSize;
			sum += local;
		}
	}
	else if(integrationType == 3)
	{
		printf("Method: Trapazoidal\n");
		for(i = 0; i < intervals; i ++)
		{
			//calculate endpoints
			double left = start + (i*stepSize);
			double right = start + (i*stepSize + stepSize);
			//calculate area
			double local = (( f(left) + f(right) ) / 2 ) * stepSize;
			sum += local;
		}
	}
	else if(integrationType == 4)
	{	
		printf("Method: Simpsons\n");
		int flip;
		float stepThree = stepSize/3.0;
		sum = f(0);
		for(i = 1; i < intervals-1; i ++)
		{
			flip = (2 * pow(2,i%2));
			double temp = start + (i*stepSize);
			double local = f(temp) * flip;
			sum += local;
		}
		double temp = start + (intervals*stepSize);
		double local = f(temp);
		sum+=local;
		sum = stepThree * sum;
	}

	//get finisihng time and compute difference
	clock_gettime(CLOCK_MONOTONIC, &finish);
	total = diff(start_clock, finish);
	

	//print the sum error and time the program took
	printf("Sum: %.20f\n", sum);
    printf("Error: %.20f\n", 2.0-sum);
	printf("Time: %ld.%.9ld\n", total.tv_sec ,total.tv_nsec);


	return 0;
}