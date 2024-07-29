#include <stdio.h>
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#include <mpi.h>
#include <stdlib.h>

int min(int a, int b)
{
    return (a < b) ? a : b;
}


int main(int argc, char *argv[])
{
    int n=4;
    // printf("Enter n: ");
    // scanf("%d", &n);
    // int DO[n][n];
    // int D[n][n];


    int **DO = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        DO[i] = (int *)malloc(n * sizeof(int));
    }

     int **D = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        D[i] = (int *)malloc(n * sizeof(int));
    }


    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                DO[i][j] = 0; // 0 at diagonal
            }
            else
            {
                printf("Enter value at D0[%d][%d]: ", i, j);
                scanf("%d", &DO[i][j]); // take matrix input
            }
        }
    } // algorithm
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                D[i][j] = min(DO[i][j], (DO[i][k] + DO[k][j]));
            }
        }
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                DO[i][j] = D[i][j];
            }
        }
    }
    printf("Printing D matrix\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", D[i][j]);
        }
        printf("\n");
    }
}