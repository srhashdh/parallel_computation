#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 1024
#define N 1024
#define L 1024

void initial(int **arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] = 1;
        }
    }
}

int main() {
    int **A, **B, **C1;

    A = (int **)malloc(M * sizeof(int *));
    B = (int **)malloc(N * sizeof(int *));
    C1 = (int **)malloc(M * sizeof(int *));
    for (int i = 0; i < M; i++) {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(L * sizeof(int));
        C1[i] = (int *)malloc(L * sizeof(int));
    }

    initial(A, M, N);
    initial(B, N, L);

    int start1 = clock();
    for(int r = 0; r < 19; r++){
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < L; k++) {
                for (int j = 0; j < N; j++) {
                    C1[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    int end1 = clock();
    printf("Time 3: %d\n", end1 - start1);

    for (int i = 0; i < M; i++) {
        free(A[i]);
        free(B[i]);
        free(C1[i]);
    }
    free(A);
    free(B);
    free(C1);

    return 0;
}

