#include <cmath>
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
  int sqrt_comm_sz = sqrt(comm_sz);

  // every row a comm
  MPI_Comm row_comm;
  int my_row_rank;
  MPI_Comm_split(MPI_COMM_WORLD, my_rank / sqrt_comm_sz, my_rank, &row_comm);

  // first col a comm
  MPI_Group world_group;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);

  int *ranks = new int[sqrt_comm_sz];
  for (int i = 0; i < sqrt_comm_sz; i++) {
    ranks[i] = i * sqrt_comm_sz;
  }

  MPI_Group col_group;
  MPI_Group_incl(world_group, sqrt_comm_sz, ranks, &col_group);

  // Create a new communicator based on the group
  MPI_Comm col_comm;
  MPI_Comm_create_group(MPI_COMM_WORLD, col_group, 0, &col_comm);

  MPI_Datatype block_sub_mpi_t;
  MPI_Datatype vect_mpi_t;

  int local_n = n / sqrt_comm_sz;
  double *local_matrix = new double[local_n * local_n];
  double *local_vector = new double[local_n];
  double *local_ans = new double[local_n];
  double *local_sum = new double[local_n];

  if (n % comm_sz || (sqrt_comm_sz * sqrt_comm_sz != comm_sz)) {
    if (my_rank == 0) {
      printf("Bad n and comm_sz!\n");
    }
    MPI_Finalize();
    return;
  }

  /* n blocks each containing local_n elements */
  /* The start of each block is n doubles beyond the preceding block */
  MPI_Type_vector(local_n, local_n, n, MPI_DOUBLE, &vect_mpi_t);

  /* Resize the new type so that it has the extent of local_n doubles */
  MPI_Type_create_resized(vect_mpi_t, 0, local_n * sizeof(double),
                          &block_sub_mpi_t);
  MPI_Type_commit(&block_sub_mpi_t);

  double *flat_matrix = NULL;
  if (my_rank == 0) {
    gen(n, &matrix, &vector);
    flat_matrix = new double[n * n];
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        flat_matrix[i * n + j] = matrix[i][j];
      }
    }
    printf("Now begins\n");
  }

  double time_before_scatter = MPI_Wtime();
  int *sendcounts = new int[comm_sz];
  int *displs = new int[comm_sz];
  for (int i = 0; i < sqrt_comm_sz; i++) {
    for (int j = 0; j < sqrt_comm_sz; j++) {
      sendcounts[i * sqrt_comm_sz + j] = 1;
      displs[i * sqrt_comm_sz + j] = i * sqrt_comm_sz * local_n + j;
    }
  }
  MPI_Scatterv(flat_matrix, sendcounts, displs, block_sub_mpi_t, local_matrix,
               local_n * local_n, MPI_DOUBLE, 0, comm);
  for (int i = 0; i < sqrt_comm_sz; i++) {
    for (int j = 0; j < sqrt_comm_sz; j++) {
      sendcounts[i * sqrt_comm_sz + j] = local_n;
      displs[i * sqrt_comm_sz + j] = j * local_n;
    }
  }
  MPI_Scatterv(vector, sendcounts, displs, MPI_DOUBLE, local_vector, local_n,
               MPI_DOUBLE, 0, comm);
  double time_after_scatter = MPI_Wtime();
  MPI_Barrier(comm);
  double time_before_calc = MPI_Wtime();

  for (int i = 0; i < local_n; i++) {
    local_sum[i] = 0.0;
    for (int j = 0; j < local_n; j++) {
      local_sum[i] += local_matrix[i * local_n + j] * local_vector[j];
    }
  }

  MPI_Reduce(local_sum, local_ans, local_n, MPI_DOUBLE, MPI_SUM, 0, row_comm);
  MPI_Barrier(comm);
  double time_after_calc = MPI_Wtime();

  if (my_rank == 0) {
    answer = new double[n];
    MPI_Gather(local_ans, local_n, MPI_DOUBLE, answer, local_n, MPI_DOUBLE, 0,
               col_comm);
    double time_before_serial = MPI_Wtime();
    serial(n, matrix, vector, &expected);
    double time_after_serial = MPI_Wtime();
    double loss = 0;
    for (int i = 0; i < n; i++) {
      loss += (answer[i] - expected[i]) * (answer[i] - expected[i]);
    }
    printf("loss: %lf\n", loss);
    printf("time(scatter): %lf\n", time_after_scatter - time_before_scatter);
    printf("time(calc): %lf\n", time_after_calc - time_before_calc);
    printf("time(serial): %lf\n", time_after_serial - time_before_serial);
  } else if (my_rank % sqrt_comm_sz == 0) {
    MPI_Gather(local_ans, local_n, MPI_DOUBLE, answer, local_n, MPI_DOUBLE, 0,
               col_comm);
  }
  MPI_Finalize();
}
