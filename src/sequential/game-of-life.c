#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void mallocGrid(int ***g, int g_size);
void print2DArray(int **g, int g_size);
void randPopulation(int ***g, int g_size);
void createGhostCells(int ***g, int g_size);
int getNeighbors(int ***grid, int i, int j);
void crossRules(int ***grid, int ***new_grid, int grid_size);
void gameLoop(int ***grid, int ***new_grid, int grid_size, int generations);
void countTotalAlives(int **grid, int grid_size);
void swapGrids(int ***grid, int ***new_grid);

// Cria um grid
void mallocGrid(int ***g, int g_size) {
  *g = (int **)malloc(sizeof(int *) * (g_size + 2));
  for (int i = 0; i < g_size + 2; i++) {
    (*g)[i] = (int *)malloc(sizeof(int) * (g_size + 2));
  }
}

// Inicializa uma população aleatória
void randPopulation(int ***g, int g_size) {
  for (int i = 1; i <= g_size; i++) {
    for (int j = 1; j <= g_size; j++) {
      (*g)[i][j] = rand() % 2;
    }
  }
}

// Cria dados duplicados nas bordas do grid
void createGhostCells(int ***g, int g_size) {
  for (int i = 1; i <= g_size; i++) {
    (*g)[i][0] = (*g)[i][g_size];
    (*g)[i][g_size + 1] = (*g)[i][1];
  }
  for (int j = 0; j <= g_size + 1; j++) {
    (*g)[0][j] = (*g)[g_size][j];
    (*g)[g_size + 1][j] = (*g)[1][j];
  }
}

void print2DArray(int **g, int g_size) {
  for (int i = 0; i < g_size + 2; i++) {
    for (int j = 0; j < g_size + 2; j++) {
      printf("%d ", g[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

// Retorna a quantidade de vizinhos de um pixel
int getNeighbors(int ***grid, int i, int j) {
  int numNeighbors;
  numNeighbors = (*grid)[i + 1][j] + (*grid)[i - 1][j]           // lower upper
                 + (*grid)[i][j + 1] + (*grid)[i][j - 1]         // right left
                 + (*grid)[i + 1][j + 1] + (*grid)[i - 1][j - 1] // diagonals
                 + (*grid)[i - 1][j + 1] + (*grid)[i + 1][j - 1];

  return numNeighbors;
}

void crossRules(int ***grid, int ***new_grid, int grid_size) {
  for (int i = 1; i <= grid_size; i++) {
    for (int j = 1; j <= grid_size; j++) {
      int numNeighbors = getNeighbors(grid, i, j);
      if ((*grid)[i][j] == 1 && numNeighbors < 2)
        (*new_grid)[i][j] = 0;
      else if ((*grid)[i][j] == 1 && (numNeighbors == 2 || numNeighbors == 3))
        (*new_grid)[i][j] = 1;
      else if ((*grid)[i][j] == 1 && numNeighbors > 3)
        (*new_grid)[i][j] = 0;
      else if ((*grid)[i][j] == 0 && numNeighbors == 3)
        (*new_grid)[i][j] = 1;
      else
        (*new_grid)[i][j] = (*grid)[i][j];
    }
  }
}

void swapGrids(int ***grid, int ***new_grid) {
  int **tmpGrid = (*grid);
  (*grid) = (*new_grid);
  (*new_grid) = tmpGrid;
}

void gameLoop(int ***grid, int ***new_grid, int grid_size, int generations) {
  for (int gen = 0; gen < generations; gen++) {
    createGhostCells(grid, grid_size);
    crossRules(grid, new_grid, grid_size);
    countTotalAlives((*grid), grid_size);
    swapGrids(grid, new_grid);
  }
}

void countTotalAlives(int **grid, int grid_size) {
  // Sum up alive cells and print results
  int total = 0;
  for (int i = 1; i <= grid_size; i++) {
    for (int j = 1; j <= grid_size; j++) {
      total += grid[i][j];
    }
  }
  printf("Total Alive: %d\n", total);
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

  int **grid;
  int **new_grid;
  mallocGrid(&grid, grid_size);
  mallocGrid(&new_grid, grid_size);
  srand((unsigned long)time(NULL));
  randPopulation(&grid, grid_size);
  gameLoop(&grid, &new_grid, grid_size, generations);
  countTotalAlives(grid, grid_size);
  free(grid);
  free(new_grid);
  return 0;
}
