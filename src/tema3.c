#include "tema3.h"



int main (int argc, char *argv[])
{
    int procs, coordinator, rank, start_pos, dim_vec, nr_workers, start, end;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *starts = (int *) malloc(procs * sizeof(int));
    int *ends = (int *) malloc(procs * sizeof(int));
    int *v;
    int **workers = (int **) malloc(CLUSTERS * sizeof(int *));
    int *clusters_num_workers = (int *) malloc(CLUSTERS * sizeof(int));

    // If the current process represents a coordinator
    if (rank < 4) {
        get_cluster_for_leader(rank, workers, clusters_num_workers);
        if (rank == 0) {
            communication_rank_zero(rank, workers, clusters_num_workers);
        } else if (rank == 3) {
            communication_rank_three(rank, workers, clusters_num_workers);
        } else if (rank == 2) {
            communication_rank_two(rank, workers,clusters_num_workers);
        } else if (rank == 1) {
            communication_rank_one(rank, workers, clusters_num_workers);
        }
    } else {
        MPI_Recv(&coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(clusters_num_workers, CLUSTERS, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < CLUSTERS; i++) {
            workers[i] = (int *) malloc(clusters_num_workers[i] * sizeof(int));
            MPI_Recv(workers[i], clusters_num_workers[i], MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    printf("%d -> ", rank);
    for (int i = 0; i < CLUSTERS; i++) {
        printf("%d:", i);
        for (int j = 0; j < clusters_num_workers[i] - 1; j++) {
            printf("%d,", workers[i][j]);
        }
        printf("%d ", workers[i][clusters_num_workers[i] - 1]);
    }
    printf("\n");

    // Calculations
    if (rank == 0) {
        dim_vec = atoi(argv[1]);
        v = (int *) malloc(dim_vec * sizeof(int));
        
        for (int i = 0; i < dim_vec; i++) {
            v[i] = dim_vec - i - 1;
        }
        
        nr_workers = procs - CLUSTERS;
        
        for (int i = 0; i < CLUSTERS; i++) {
            for (int j = 0; j < clusters_num_workers[i]; j++) {
                starts[workers[i][j]] = (workers[i][j] - CLUSTERS) * (double)dim_vec / nr_workers;
                ends[workers[i][j]] = min((workers[i][j] - CLUSTERS + 1) * (double)dim_vec / nr_workers, dim_vec);
            }
        }

        for_each_coordinator(rank, workers, clusters_num_workers, v, dim_vec, starts,
                                ends, procs);

        MPI_Send(&dim_vec, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 3, 0, MPI_COMM_WORLD);
        MPI_Send(starts, procs, MPI_INT, 3, 0, MPI_COMM_WORLD);
        MPI_Send(ends, procs, MPI_INT, 3, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 3);

        MPI_Recv(&dim_vec, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v, dim_vec, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        print_final_vec(v, dim_vec);

    } else if(rank == 3) {
        MPI_Recv(&dim_vec, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v = (int *) malloc(dim_vec * sizeof(int));
        MPI_Recv(v, dim_vec, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(starts, procs, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(ends, procs, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for_each_coordinator(rank, workers, clusters_num_workers, v, dim_vec, starts,
                                ends, procs);

        MPI_Send(&dim_vec, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 2, 0, MPI_COMM_WORLD);

        MPI_Send(starts, procs, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(ends, procs, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);

        MPI_Recv(&dim_vec, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v, dim_vec, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Send(&dim_vec, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 0);
    } else if (rank == 2) {
        MPI_Recv(&dim_vec, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v = (int *) malloc(dim_vec * sizeof(int));
        MPI_Recv(v, dim_vec, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(starts, procs, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(ends, procs, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for_each_coordinator(rank, workers, clusters_num_workers, v, dim_vec, starts,
                                ends, procs);

        MPI_Send(&dim_vec, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 1, 0, MPI_COMM_WORLD);

        MPI_Send(starts, procs, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(ends, procs, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 1);

        MPI_Recv(&dim_vec, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v, dim_vec, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Send(&dim_vec, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);

    } else if (rank == 1) {
        MPI_Recv(&dim_vec, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v = (int *) malloc(dim_vec * sizeof(int));
        MPI_Recv(v, dim_vec, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(starts, procs, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(ends, procs, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for_each_coordinator(rank, workers, clusters_num_workers, v, dim_vec, starts,
                                ends, procs);

        MPI_Send(&dim_vec, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(v, dim_vec, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);


    } else {
        MPI_Recv(&dim_vec, 1, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v = (int *) malloc(dim_vec * sizeof(int));
        MPI_Recv(v, dim_vec, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&procs, 1, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(starts, procs, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&procs, 1, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(ends, procs, MPI_INT, coordinator, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = starts[rank]; i < ends[rank]; i++) {
            v[i] *= 5;
        }

        MPI_Send(v, dim_vec, MPI_INT, coordinator, 1, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, coordinator);
    }

    for (int i = 0; i < CLUSTERS; i++) {
        free(workers[i]);
    }
    free(workers);
    free(clusters_num_workers);

    MPI_Finalize();
}
