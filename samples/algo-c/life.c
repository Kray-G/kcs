#include <stdio.h>
#include <stdlib.h>

#define N  22
#define M  78
#define ALLDISP 1
#define LAST 1000
char a[N + 2][M + 2], b[N + 2][M + 2];

int main(void)
{
    int i, j, g;

    a[N/2][M/2] = a[N/2-1][M/2] = a[N/2+1][M/2]
        = a[N/2][M/2-1] = a[N/2-1][M/2+1] = 1;
    for (g = 1; g <= LAST; g++) {
        if (ALLDISP || g == LAST) printf("Generation %4d\n", g);
        for (i = 1; i <= N; i++) {
            for (j = 1; j <= M; j++)
                if (a[i][j]) {
                    if (ALLDISP || g == LAST) printf("*");
                    b[i-1][j-1]++;  b[i-1][j]++;  b[i-1][j+1]++;
                    b[i  ][j-1]++;                b[i  ][j+1]++;
                    b[i+1][j-1]++;  b[i+1][j]++;  b[i+1][j+1]++;
                } else if (ALLDISP || g == LAST) printf(".");
            if (ALLDISP || g == LAST) printf("\n");
        }
        for (i = 0; i <= N + 1; i++)
            for (j = 0; j <= M + 1; j++) {
                if (b[i][j] != 2) a[i][j] = (b[i][j] == 3);
                b[i][j] = 0;
            }
    }
    return 0;
}
