#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

int main(int argc, char *argv[])
{
	if(argc < 3){
		printf("Usage: ./mmm <mode> [num threads] <size>\n");
		return -1;
	}

	char *mode = argv[1];

	double clockstart, clockend;
	
	if(strcmp(mode, "S") == 0){
		if(argc > 3){
			printf("Usage: ./mmm <mode> [num threads] <size>\n");
			return -1;
		}

		MATRIX_SIZE = atoi(argv[2]);

		printf("========\n");
		printf("mode: sequential\n");
		printf("thread count: 1\n");
		printf("size: %d\n", MATRIX_SIZE);
		printf("========\n");
		
		mmm_init();

		double totalTime = (double) 0;
		for(int n=0; n<NUM_RUNS+1; n++){
			clockstart = rtclock(); // start clock: stuff I want to clock
			mmm_seq();
			clockend = rtclock(); // end clock: stuff I want to clock
			double seqTime = clockend - clockstart;
			if(n>0){
				totalTime += seqTime;
			}
			mmm_reset(C);
		}

		double avgTime = totalTime/NUM_RUNS;
		printf("Sequantial Time (avg of 3 runs): %f sec\n", avgTime);
		
	}
	else if(strcmp(mode, "P") == 0){
		if(argc < 4){
			printf("Usage: ./mmm <mode> [num threads] <size>\n");
			return -1;
		}

		NUM_THREADS = atoi(argv[2]);
		MATRIX_SIZE = atoi(argv[3]);

		printf("========\n");
		printf("mode: parallel\n");
		printf("thread count: %d\n", NUM_THREADS);
		printf("size: %d\n", MATRIX_SIZE);
		printf("========\n");

		mmm_init();

		double totalSeqTime = (double) 0;
		for(int n=0; n<NUM_RUNS+1; n++){
			clockstart = rtclock(); // start clock: stuff I want to clock
			mmm_seq();
			clockend = rtclock(); // end clock: stuff I want to clock
			double seqTime = clockend - clockstart;
			if(n==0){
				for(int i=0; i<MATRIX_SIZE; i++){
					for(int j=0; j<MATRIX_SIZE; j++){
						verifyMatrix[i][j] = C[i][j];
					}
				}
			}
			else{
				totalSeqTime += seqTime;
			}
			mmm_reset(C);
		}

		double avgSeqTime = totalSeqTime/NUM_RUNS;
		printf("Sequantial Time (avg of 3 runs): %f sec\n", avgSeqTime);

		//Resets value to make sure mmm_par() is correctly evaluating
		mmm_reset(C);

		double maxError;
		double totalParTime = (double) 0;
		for(int n=0; n<NUM_RUNS+1; n++){
			clockstart = rtclock(); // start clock: stuff I want to clock

			pthread_t threads[NUM_THREADS];

			//Prepare threads
			long i;
			thread_args args[NUM_THREADS];
			for(i=0; i<NUM_THREADS; i++){
				//prepare arguments for a thread
				args[i].tid = i;
				args[i].rowBegin = i * (MATRIX_SIZE / NUM_THREADS);
				args[i].rowEnd = (i+1) * (MATRIX_SIZE / NUM_THREADS);

				if(i == NUM_THREADS-1){
					args[i].rowEnd = MATRIX_SIZE;
				}

				pthread_create(&threads[i], NULL, mmm_par, &args[i]);
			}

			//Reap threads, collect parts of matrix
			for(i=0; i<NUM_THREADS; i++){
				pthread_join(threads[i], NULL);
			}

			clockend = rtclock(); // end clock: stuff I want to clock
			double parTime = clockend - clockstart;

			if(n==0){
				maxError = mmm_verify();
			}
			else{
				totalParTime += parTime;
			}
		}

		
		double avgParTime = totalParTime/NUM_RUNS;
		printf("Parallel Time (avg of 3 runs): %f sec\n", avgParTime);



		double speedup = avgSeqTime/avgParTime;
		printf("Speedup: %f\n", speedup);

		printf("Verifying... largest error between parallel and dequential matrix: %f\n", maxError);

	}
	else{
		printf("Mode must be 'S' or 'P'\n");
		printf("Usage: ./mmm <mode> [num threads] <size>\n");
		return -1;
	}


	mmm_freeup();
	return 0;
}
