#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <omp.h>

#define N 800

float A[N][N], B[N][N], C1[N][N], C2[N][N];

int main () {
    int i, j, k;

    srand ( time(NULL) );
    for(i=0; i<N; i++) {
        for(j=0; j<N; j++) {
            A[i][j] = (rand()%10);
            B[i][j] = (rand()%10);
        }
    }
   
    /* sequential version. For testing purposes only */
    /*for(i=0; i<N; i++) {
          for(j=0; j<N; j++) {
              C1[i][j] = 0.;
              for(k=0; k<N; k++) {
                  C1[i][j] += A[i][k]*B[k][j];
              }
          }
    }*/

    /* parallel version using OpenMP */
    float m_result;
    for(i=0; i<N; i++) {
        for(j=0; j<N; j++) {
            m_result = 0.0;
            #pragma omp parallel for reduction(+:m_result)
            for(k=0; k<N; k++) {
                m_result += A[i][k]*B[k][j];
            }
            C2[i][j] = m_result;
        }
    }

    /* verify that calculations are correct */
    /*for(i=0; i<N; i++) {
        for(j=0; j<N; j++)
            assert(C1[i][j] == C2[i][j]);
    }*/
}

