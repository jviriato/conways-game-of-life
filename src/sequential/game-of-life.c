#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void mallocGrid(int ***g, int g_size);
void randPopulation(int ***g, int g_size);
void print2DArray(int **g, int g_size);

void mallocGrid(int ***g, int g_size) {
  *g = (int **)malloc(sizeof(int *) * (g_size + 2));
  for (int i = 0; i < g_size + 2; i++) {
    (*g)[i] = (int *)malloc(sizeof(int) * (g_size + 2));
  }
}

void randPopulation(int ***g, int g_size) {
  for (int i = 1; i <= g_size; i++) {
    for (int j = 1; j <= g_size; j++) {
      (*g)[i][j] = rand() % 2;
    }
  }
}

void print2DArray(int **g, int g_size) {
  for (int i = 1; i <= g_size; i++) {
    for (int j = 1; j <= g_size; j++) {
      printf("%d ", g[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char const *argv[]) {
  int generations, grid_size;
  if (argc == 3) {
    generations = atoi(argv[1]);
    grid_size = atoi(argv[2]);
  } else {
    printf("args: generations and grid_size");
    return -1;
  }
  // Malloca o mundo
  int **grid;
  int **new_grid;
  mallocGrid(&grid, grid_size);
  mallocGrid(&new_grid, grid_size);

  // População inicial randômica
  srand((unsigned long)time(NULL));
  randPopulation(&grid, grid_size);
  print2DArray(grid, grid_size);

  free(grid);
  free(new_grid);
  return 0;
}
