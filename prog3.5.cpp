#include <ctime>
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * Modify the "multiply, run" to implement your parallel algorihtm.
 * Compile:
 *      this is a c++ style code
 */
using namespace std;

void serial(int n, double **matrix, double *vector, double **result);
void gen(int n, double ***matrix, double **vector);
void print(int n, double **matrix, double *vector);
void free(int n, double **matrix, double *vector);
void run(int n, double **matrix, double *vector);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " n" << endl;
    return -1;
  }
  double **matrix;
  double *vector;
  int n = atoi(argv[1]);
  run(n, matrix, vector);
}

void serial(int n, double **matrix, double *vector, double **result) {
  /*
   * It is a serial algorithm to
   * get the true value of matrix-vector multiplication
   * please calculation the difference between true value and the value you
   * obtain
   *
   */
  (*result) = new double[n];
  for (int i = 0; i < n; i++) {
    (*result)[i] = 0.0;
  }

  for (int i = 0; i < n; i++) {
    double temp = 0.0;
    for (int j = 0; j < n; j++) {
      temp += matrix[i][j] * vector[j];
    }
    (*result)[i] = temp;
  }
}

void gen(int n, double ***matrix, double **vector) {
  /*
   *  generate random matrix and vector,
   *  In order to debug conveniently, you can define a const matrix and vector
   *  but I will check your answer based on random matrix and vector
   */
  (*matrix) = new double *[n];
  srand((unsigned)time(0));
  for (int i = 0; i < n; i++) {
    (*matrix)[i] = new double[n];
    for (int j = 0; j < n; j++) {
      (*matrix)[i][j] = -1 + rand() * 1.0 / RAND_MAX * 2;
    }
  }
  (*vector) = new double[n];
  for (int i = 0; i < n; i++) {
    (*vector)[i] = -1 + rand() * 1.0 / RAND_MAX * 2;
  }
}

void print(int n, double **matrix, double *vector) {
  for (int i = 0; i < n; i++) {
    cout << vector[i] << endl;
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }
}

void free(int n, double **matrix, double *vector) {
  delete[] vector;
  for (int i = 0; i < n; i++)
    delete[] matrix[i];
  delete[] matrix;
}

void run(int n, double **matrix, double *vector) {
  /*
   * Description:
   * data partition, communication, calculation based on MPI programming in this
   * function.
   *
   * 1. call gen() on one process to generate the random matrix and vecotr.
   * 2. distribute the data to other processes.
   * 3. Implement matrix-vector mutiply
   * 4. calculate the diffenence between product vector and the value of
   * serial(), I'll check this answer.
   */
  double *answer, *expected;
  int my_rank, comm_sz;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  MPI_Datatype block_col_mpi_t;
  MPI_Datatype vect_mpi_t;

  int local_n = n / comm_sz;
  double *local_matrix = new double[local_n * n];
  double *local_vector = new double[local_n];
  double *local_sum = new double[n];
  double *local_ans = new double[local_n];

  if (n % comm_sz) {
    if (my_rank == 0) {
      printf("Bad n and comm_sz!\n");
    }
    MPI_Finalize();
    return;
  }

  /* n blocks each containing local_n elements */
  /* The start of each block is n doubles beyond the preceding block */
  MPI_Type_vector(n, local_n, n, MPI_DOUBLE, &vect_mpi_t);

  /* Resize the new type so that it has the extent of local_n doubles */
  MPI_Type_create_resized(vect_mpi_t, 0, local_n * sizeof(double),
                          &block_col_mpi_t);
  MPI_Type_commit(&block_col_mpi_t);

  double *flat_matrix = NULL;
  if (my_rank == 0) {
    gen(n, &matrix, &vector);
    flat_matrix = new double[n * n];
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        flat_matrix[i * n + j] = matrix[i][j];
      }
    }
  }

  double time_before_scatter = MPI_Wtime();
  MPI_Scatter(flat_matrix, 1, block_col_mpi_t, local_matrix, n * local_n,
              MPI_DOUBLE, 0, comm);
  MPI_Scatter(vector, local_n, MPI_DOUBLE, local_vector, local_n, MPI_DOUBLE, 0,
              comm);
  double time_after_scatter = MPI_Wtime();
  MPI_Barrier(comm);
  double time_before_calc = MPI_Wtime();

  for (int i = 0; i < n; i++) {
    local_sum[i] = 0.0;
    for (int j = 0; j < local_n; j++) {
      local_sum[i] += local_matrix[i * local_n + j] * local_vector[j];
    }
  }

  int *recv_counts = new int[comm_sz];
  for (int i = 0; i < comm_sz; i++) {
    recv_counts[i] = local_n;
  }

  MPI_Reduce_scatter(local_sum, local_ans, recv_counts, MPI_DOUBLE, MPI_SUM,
                     comm);
  MPI_Barrier(comm);
  double time_after_calc = MPI_Wtime();

  if (my_rank == 0) {
    answer = new double[n];
    MPI_Gather(local_ans, local_n, MPI_DOUBLE, answer, local_n, MPI_DOUBLE, 0,
               comm);
    serial(n, matrix, vector, &expected);
    double loss = 0;
    for (int i = 0; i < n; i++) {
      loss += (answer[i] - expected[i]) * (answer[i] - expected[i]);
    }
    printf("loss: %lf\n", loss);
    printf("time(scatter): %lf\n", time_after_scatter - time_before_scatter);
    printf("time(calc): %lf\n", time_after_calc - time_before_calc);
  } else {
    MPI_Gather(local_ans, local_n, MPI_DOUBLE, answer, local_n, MPI_DOUBLE, 0,
               comm);
  }
  MPI_Finalize();
}
