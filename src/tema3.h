#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUSTERS 4

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}



void get_cluster_for_leader(int rank, int **workers, int *clusters_num_workers) {
    FILE *clusters_file;

    char clusters_file_name[20];
    sprintf(clusters_file_name, "cluster%d.txt", rank);
    clusters_file = fopen(clusters_file_name, "r");
    fscanf(clusters_file, "%d", &clusters_num_workers[rank]);
    workers[rank] = (int *) malloc(clusters_num_workers[rank] * sizeof(int));
    for (int i = 0; i < clusters_num_workers[rank]; i++) {
        fscanf(clusters_file, "%d", &workers[rank][i]);
    }
}

void send_topology_to_workers(int rank, int **workers, int *clusters_num_workers) {
    for (int i = 0; i < clusters_num_workers[rank]; i++) {
        MPI_Send(&rank, 1, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
        MPI_Send(clusters_num_workers, CLUSTERS, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
        for (int j = 0; j < CLUSTERS; j++) {
            MPI_Send(workers[j], clusters_num_workers[j], MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[rank][i]);
        }
    }
}

void send_vec_to_all_workers(int rank, int **workers, int *clusters_num_workers, int *v, int dim_vec) {
    // Sends vec to his workers
    for (int i = 0; i < clusters_num_workers[rank]; i++) {
        MPI_Send(&dim_vec, 1, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, workers[rank][i]);
    }
}

void recv_vec_from_workers(int rank, int **workers, int *clusters_num_workers, int *v, int dim_vec, int *starts, int *ends) {
    // Receive vec from workers
    int *v_copy = (int *) malloc(dim_vec * sizeof(int));
    for (int i = 0; i < clusters_num_workers[rank]; i++) {
        MPI_Recv(v_copy, dim_vec, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = starts[workers[rank][i]]; j < ends[workers[rank][i]]; j++) {
            v[j] = v_copy[j];
        }
    }
}

void print_final_vec(int *v, int dim_vec) {
    printf("Rezultat: ");
    for (int i = 0; i < dim_vec; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

void for_each_coordinator(int rank, int **workers, int *clusters_num_workers, int *v, int dim_vec,
                            int *starts, int *ends, int procs) {

    send_vec_to_all_workers(rank, workers, clusters_num_workers, v, dim_vec);
    send_vec_to_all_workers(rank, workers, clusters_num_workers, starts, procs);
    send_vec_to_all_workers(rank, workers, clusters_num_workers, ends, procs);

    recv_vec_from_workers(rank, workers, clusters_num_workers, v, dim_vec, starts, ends);
}

void communication_rank_zero(int rank, int **workers, int *clusters_num_workers) {
    MPI_Send(&clusters_num_workers[rank], 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Send(workers[rank], clusters_num_workers[rank], MPI_INT, 3, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, 3);
    
    MPI_Recv(&clusters_num_workers[1], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[1] = (int *) calloc(clusters_num_workers[1], sizeof(int));
    MPI_Recv(workers[1], clusters_num_workers[1], MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[2], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[2] = (int *) calloc(clusters_num_workers[2], sizeof(int));
    MPI_Recv(workers[2], clusters_num_workers[2], MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[3], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[3] = (int *) calloc(clusters_num_workers[3], sizeof(int));
    MPI_Recv(workers[3], clusters_num_workers[3], MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    send_topology_to_workers(rank, workers, clusters_num_workers);
}

void communication_rank_three(int rank, int **workers, int *clusters_num_workers) {
    MPI_Recv(&clusters_num_workers[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[0] = (int *) malloc(clusters_num_workers[0] * sizeof(int));
    MPI_Recv(workers[0], clusters_num_workers[0], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&clusters_num_workers[0], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Send(workers[0], clusters_num_workers[0], MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[3], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Send(workers[3], clusters_num_workers[3], MPI_INT, 2, 0, MPI_COMM_WORLD);

    printf("M(%d,%d)\n", rank, 2);

    MPI_Recv(&clusters_num_workers[1], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[1] = (int *) calloc(clusters_num_workers[1], sizeof(int));
    MPI_Recv(workers[1], clusters_num_workers[1], MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[2], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[2] = (int *) calloc(clusters_num_workers[2], sizeof(int));
    MPI_Recv(workers[2], clusters_num_workers[2], MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(&clusters_num_workers[1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(workers[1], clusters_num_workers[1], MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[2], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(workers[2], clusters_num_workers[2], MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[3], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(workers[3], clusters_num_workers[3], MPI_INT, 0, 0, MPI_COMM_WORLD);

    printf("M(%d,%d)\n", rank, 0);
    

    send_topology_to_workers(rank, workers, clusters_num_workers);
}

void communication_rank_two(int rank, int **workers, int *clusters_num_workers) {
    MPI_Recv(&clusters_num_workers[0], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[0] = (int *) malloc(clusters_num_workers[0] * sizeof(int));
    MPI_Recv(workers[0], clusters_num_workers[0], MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[3], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[3] = (int *) malloc(clusters_num_workers[3] * sizeof(int));
    MPI_Recv(workers[3], clusters_num_workers[3], MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(&clusters_num_workers[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(workers[0], clusters_num_workers[0], MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[3], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(workers[3], clusters_num_workers[3], MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[2], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(workers[2], clusters_num_workers[2], MPI_INT, 1, 0, MPI_COMM_WORLD);

    printf("M(%d,%d)\n", rank, 1);

    MPI_Recv(&clusters_num_workers[1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[1] = (int *) calloc(clusters_num_workers[1], sizeof(int));
    MPI_Recv(workers[1], clusters_num_workers[1], MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(&clusters_num_workers[1], 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Send(workers[1], clusters_num_workers[1], MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Send(&clusters_num_workers[2], 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Send(workers[2], clusters_num_workers[2], MPI_INT, 3, 0, MPI_COMM_WORLD);

    printf("M(%d,%d)\n", rank, 3);

    send_topology_to_workers(rank, workers, clusters_num_workers);
}

void communication_rank_one(int rank, int **workers, int *clusters_num_workers) {
    MPI_Recv(&clusters_num_workers[0], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[0] = (int *) calloc(clusters_num_workers[0], sizeof(int));
    MPI_Recv(workers[0], clusters_num_workers[0], MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[3], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[3] = (int *) calloc(clusters_num_workers[3], sizeof(int));
    MPI_Recv(workers[3], clusters_num_workers[3], MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&clusters_num_workers[2], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    workers[2] = (int *) calloc(clusters_num_workers[2], sizeof(int));
    MPI_Recv(workers[2], clusters_num_workers[2], MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(&clusters_num_workers[1], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Send(workers[1], clusters_num_workers[1], MPI_INT, 2, 0, MPI_COMM_WORLD);

    printf("M(%d,%d)\n", rank, 2);

    send_topology_to_workers(rank, workers, clusters_num_workers);
}

