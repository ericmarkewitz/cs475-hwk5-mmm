#ifndef MMM_H_
#define MMM_H_

typedef struct thread_args
{
    int tid;
    int rowBegin;
    int rowEnd;
} thread_args;


// globals (anything here would be shared with all threads) */
// I would declare the pointers to the matrices here (i.e., double **A, **B, **C),
// as well as the size of the matrices, etc.
#define MAX_INPUT 100
#define NUM_RUNS 3
int MATRIX_SIZE;
int NUM_THREADS;

double **A;
double **B;
double **C;
double **verifyMatrix;




void mmm_print();
void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void *mmm_par(void *args);
double mmm_verify();

#endif /* MMM_H_ */
