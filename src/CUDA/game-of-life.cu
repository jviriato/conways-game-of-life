#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>


// Número de threads por bloco
#define BLOCK_SIZE 16

/*
 * Tempo (wallclock) em microssegundos
 */
 long wtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
  }


__global__ void ghostRows(int grid_size, int *grid) {
  int id = blockDim.x * blockIdx.x + threadIdx.x + 1;

  if (id <= grid_size) {
    // copia a primeira coluna para o fim
    grid[(grid_size + 2) * (grid_size + 1) + id] = grid[(grid_size + 2) + id];
    // copia a última coluna para o topo
    grid[id] = grid[(grid_size + 2) * grid_size + id];
  }
}

__global__ void ghostCols(int grid_size, int *grid) {
  int id = blockDim.x * blockIdx.x + threadIdx.x;

  if (id <= grid_size + 1) {
    // copia a primeira coluna para a direita
    grid[id * (grid_size + 2) + grid_size + 1] = grid[id * (grid_size + 2) + 1];
    // copia a última coluna para a esquerda
    grid[id * (grid_size + 2)] = grid[id * (grid_size + 2) + grid_size];
  }
}

__global__ void crossRules(int grid_size, int *grid, int *newGrid) {
  int iy = blockDim.y * blockIdx.y + threadIdx.y + 1;
  int ix = blockDim.x * blockIdx.x + threadIdx.x + 1;
  int id = iy * (grid_size + 2) + ix;

  int numNeighbors;

  if (iy <= grid_size && ix <= grid_size) {
    numNeighbors = grid[id + (grid_size + 2)] + grid[id - (grid_size + 2)]   
                   + grid[id + 1] + grid[id - 1]                 
                   + grid[id + (grid_size + 3)] + grid[id - (grid_size + 3)]
                   + grid[id - (grid_size + 1)] + grid[id + (grid_size + 1)];

    int cell = grid[id];
    if (cell == 1 && numNeighbors < 2)
      newGrid[id] = 0;
    else if (cell == 1 && (numNeighbors == 2 || numNeighbors == 3))
      newGrid[id] = 1;
    else if (cell == 1 && numNeighbors > 3)
      newGrid[id] = 0;
    else if (cell == 0 && numNeighbors == 3)
      newGrid[id] = 1;
    else
      newGrid[id] = cell;
  }
}

int main(int argc, char *argv[]) {
  int generations, grid_size;
  if (argc == 3) {
    generations = atoi(argv[1]);
    grid_size = atoi(argv[2]);
  } else {
    printf("args: generations and grid_size");
    return -1;
  }
  int *h_grid;    
  int *d_grid;    
  int *d_newGrid; 
  int *d_tmpGrid; 

  size_t bytes =
      sizeof(int) * (grid_size + 2) *
      (grid_size + 2);
  h_grid = (int *)malloc(bytes);

  cudaMalloc(&d_grid, bytes);
  cudaMalloc(&d_newGrid, bytes);
  long start_time = wtime();

  srand((unsigned long)time(NULL));
  for (int i = 1; i <= grid_size; i++) {
    for (int j = 1; j <= grid_size; j++) {
      h_grid[i * (grid_size + 2) + j] = rand() % 2;
    }
  }
  cudaMemcpy(d_grid, h_grid, bytes, cudaMemcpyHostToDevice);

  dim3 blockSize(BLOCK_SIZE, BLOCK_SIZE, 1);
  int linGrid = (int)ceil(grid_size / (float)BLOCK_SIZE);
  dim3 gridSize(linGrid, linGrid, 1);

  dim3 cpyBlockSize(BLOCK_SIZE, 1, 1);
  dim3 cpyGridRowsGridSize((int)ceil(grid_size / (float)cpyBlockSize.x), 1, 1);
  dim3 cpyGridColsGridSize((int)ceil((grid_size + 2) / (float)cpyBlockSize.x), 1, 1);

  for (int i  = 0; i < generations; i++) {
    ghostRows<<<cpyGridRowsGridSize, cpyBlockSize>>>(grid_size, d_grid);
    ghostCols<<<cpyGridColsGridSize, cpyBlockSize>>>(grid_size, d_grid);
    crossRules<<<gridSize, blockSize>>>(grid_size, d_grid, d_newGrid);
    d_tmpGrid = d_grid;
    d_grid = d_newGrid;
    d_newGrid = d_tmpGrid;
  }

  cudaMemcpy(h_grid, d_grid, bytes, cudaMemcpyDeviceToHost);
  long end_time = wtime();
  printf("%ld usec\n", (long)(end_time - start_time));

  int total = 0;
  for (int i = 1; i <= grid_size; i++) {
    for (int j = 1; j <= grid_size; j++) {
      total += h_grid[i * (grid_size + 2) + j];
    }
  }
  printf("Total Alive: %d\n", total);


  cudaFree(d_grid);
  cudaFree(d_newGrid);
  free(h_grid);

  return 0;
}