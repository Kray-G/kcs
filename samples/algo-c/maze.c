#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define XMAX  80
#define YMAX  24
#define MAXSITE  (XMAX * YMAX / 4)
char map[XMAX + 1][YMAX + 1];
int nsite = 0;
int xx[MAXSITE], yy[MAXSITE];
int dx[4] = { 2, 0, -2,  0 };
int dy[4] = { 0, 2,  0, -2 };
int dirtable[24][4] = {
    0,1,2,3, 0,1,3,2, 0,2,1,3, 0,2,3,1, 0,3,1,2, 0,3,2,1,
    1,0,2,3, 1,0,3,2, 1,2,0,3, 1,2,3,0, 1,3,0,2, 1,3,2,0,
    2,0,1,3, 2,0,3,1, 2,1,0,3, 2,1,3,0, 2,3,0,1, 2,3,1,0,
    3,0,1,2, 3,0,2,1, 3,1,0,2, 3,1,2,0, 3,2,0,1, 3,2,1,0 };

void add(int i, int j)
{
    xx[nsite] = i;  yy[nsite] = j;  nsite++;
}

int select(int *i, int *j)
{
    int r;

    if (nsite == 0) return 0;
    nsite--;  r = (int)(nsite * (rand() / (RAND_MAX + 1.0)));
    *i = xx[r];  xx[r] = xx[nsite];
    *j = yy[r];  yy[r] = yy[nsite];  return 1;
}

int main(void)
{
    int i, j, i1, j1, d, t, *tt;

    srand((unsigned)time(NULL));
    for (i = 0; i <= XMAX; i++)
        for (j = 0; j <= YMAX; j++) map[i][j] = 1;
    for (i = 3; i <= XMAX - 3; i++)
        for (j = 3; j <= YMAX - 3; j++) map[i][j] = 0;
    map[2][3] = 0;  map[XMAX - 2][YMAX - 3] = 0;
    for (i = 4; i <= XMAX - 4; i += 2) {
        add(i, 2);  add(i, YMAX - 2);
    }
    for (j = 4; j <= YMAX - 4; j += 2) {
        add(2, j);  add(XMAX - 2, j);
    }
    while (select(&i, &j)) {
        for ( ; ; ) {
            tt = dirtable[(int)(24 * (rand() / (RAND_MAX + 1.0)))];
            for (d = 3; d >= 0; d--) {
                t = tt[d];  i1 = i + dx[t];  j1 = j + dy[t];
                if (map[i1][j1] == 0) break;
            }
            if (d < 0) break;
            map[(i + i1) / 2][(j + j1) / 2] = 1;
            i = i1;  j = j1;  map[i][j] = 1;  add(i, j);
        }
    }
    for (j = 2; j <= YMAX - 2; j++) {
        for (i = 2;i <= XMAX - 2; i++)
            if (map[i][j]) putchar('X');  else putchar(' ');
        putchar('\n');
    }
    return 0;
}
