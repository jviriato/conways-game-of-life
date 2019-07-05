#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

void mallocGrid(int ***g, int g_size);
void print2DArray(int **g, int g_size);
void randPopulation(int ***g, int g_size);
void createGhostCells(int ***g, int g_size);
int getNeighbors(int ***grid, int i, int j);
void swapGrids(int ***grid, int ***new_grid);
void countTotalAlives(int **grid, int grid_size);
void crossRules(int ***grid, int ***new_grid, int grid_size);
void gameLoop(int ***grid, int ***new_grid, int grid_size, int generations,
              unsigned char **pic);
static void writeBMP(const int x, const int y, const unsigned char *const bmp,
                     const char *const name) {
  const unsigned char bmphdr[54] = {
      66,  77, 255, 255, 255, 255, 0,   0,   0,   0,   54,  4,   0, 0,
      40,  0,  0,   0,   255, 255, 255, 255, 255, 255, 255, 255, 1, 0,
      8,   0,  0,   0,   0,   0,   255, 255, 255, 255, 196, 14,  0, 0,
      196, 14, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
  unsigned char hdr[1078];
  int i, j, c, xcorr, diff;
  FILE *f;

  xcorr = (x + 3) >> 2 << 2; // BMPs have to be a multiple of 4 pixels wide
  diff = xcorr - x;

  for (i = 0; i < 54; i++)
    hdr[i] = bmphdr[i];
  *((int *)(&hdr[18])) = xcorr;
  *((int *)(&hdr[22])) = y;
  *((int *)(&hdr[34])) = xcorr * y;
  *((int *)(&hdr[2])) = xcorr * y + 1078;
  for (i = 0; i < 256; i++) {
    j = i * 4 + 54;
    hdr[j + 0] = i; // blue
    hdr[j + 1] = i; // green
    hdr[j + 2] = i; // red
    hdr[j + 3] = 0; // dummy
  }

  f = fopen(name, "wb");
  assert(f != NULL);
  c = fwrite(hdr, 1, 1078, f);
  assert(c == 1078);
  if (diff == 0) {
    c = fwrite(bmp, 1, x * y, f);
    assert(c == x * y);
  } else {
    *((int *)(&hdr[0])) = 0; // need up to three zero bytes
    for (j = 0; j < y; j++) {
      c = fwrite(&bmp[j * x], 1, x, f);
      assert(c == x);
      c = fwrite(hdr, 1, diff, f);
      assert(c == diff);
    }
  }
  fclose(f);
}

/*
 * Tempo (wallclock) em microssegundos
 */
long wtime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000000 + t.tv_usec;
}

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

// Retorna a quantidade de vizinhos de uma célula
int getNeighbors(int ***grid, int i, int j) {
  int numNeighbors;
  numNeighbors = (*grid)[i + 1][j] + (*grid)[i - 1][j]           // lower upper
                 + (*grid)[i][j + 1] + (*grid)[i][j - 1]         // right left
                 + (*grid)[i + 1][j + 1] + (*grid)[i - 1][j - 1] // diagonals
                 + (*grid)[i - 1][j + 1] + (*grid)[i + 1][j - 1];

  return numNeighbors;
}

// Aplica as regras no mundo
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

void gameLoop(int ***grid, int ***new_grid, int grid_size, int generations,
              unsigned char **pic) {
  for (int gen = 0; gen < generations; gen++) {
    createGhostCells(grid, grid_size);
    crossRules(grid, new_grid, grid_size);
    countTotalAlives((*grid), grid_size);
    // for (int q = 0; q < grid_size; q++) {
    //   for (int t = 0; t < grid_size; t++) {
    //     (*pic)[q * grid_size + t] = (*grid)[q][t] * 255;
    //   }
    // }
    
   for (int frame = gen; frame < generations; frame++) {
      for (int row = 0; row < grid_size; row++) {
         for (int col = 0; col < grid_size; col++) {
            (*pic)[frame * grid_size * grid_size + row * grid_size + col] = (*grid)[row][col] * 255;
         }
      }
    }
    swapGrids(grid, new_grid);
  }
}

// Soma as células vivas
void countTotalAlives(int **grid, int grid_size) {
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
  unsigned char *pic = new unsigned char[generations * grid_size * grid_size];

  int **grid;
  int **new_grid;
  mallocGrid(&grid, grid_size);
  mallocGrid(&new_grid, grid_size);
  srand((unsigned long)time(NULL));
  randPopulation(&grid, grid_size);
  long start_time = wtime();
  gameLoop(&grid, &new_grid, grid_size, generations, &pic);
  long end_time = wtime();
  printf("%ld usec\n", (long)(end_time - start_time));
  countTotalAlives(grid, grid_size);
  if ((grid_size <= 256) && (generations <= 100000)) {
    for (int frame = 0; frame < generations; frame++) {
      char name[32];
      sprintf(name, "gol%d.bmp", frame + 1000);
      writeBMP(grid_size, grid_size, &pic[frame * grid_size * grid_size], name);
    }
  }

  delete[] pic;
  free(grid);
  free(new_grid);
  return 0;
}
