#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define SEND_RANK 1
#define RECV_RANK 0

void send_file(const char *filename, int dest_rank) {
    MPI_File mpi_fh;
    MPI_Status status;
    MPI_Offset filesize;
    char *buffer;
    int ierr;

    // Open the file to send in read-only mode
    ierr = MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &mpi_fh);
    if (ierr) {
    char error_string[MPI_MAX_ERROR_STRING];
    int length_of_error_string;
    MPI_Error_string(ierr, error_string, &length_of_error_string);
    fprintf(stderr, "Error opening file: %s\n", error_string);
    MPI_Abort(MPI_COMM_WORLD, ierr);
}

    // Get the size of the file
    MPI_File_get_size(mpi_fh, &filesize);

    // Allocate a buffer to hold the entire file
    buffer = (char *)malloc(filesize);
    if (!buffer) {
        perror("Unable to allocate buffer for file contents");
        MPI_File_close(&mpi_fh);
        return;
    }

    // Read the file into the buffer
    MPI_File_read(mpi_fh, buffer, filesize, MPI_BYTE, &status);

    // Send the size of the file to the receiver
    MPI_Send(&filesize, 1, MPI_OFFSET, dest_rank, 0, MPI_COMM_WORLD);

    // Send the file data to the receiver
    MPI_Send(buffer, filesize, MPI_BYTE, dest_rank, 0, MPI_COMM_WORLD);

    // Clean up
    free(buffer);
    MPI_File_close(&mpi_fh);
}

void receive_file(const char *filename, int source_rank) {
    MPI_Status status;
    MPI_Offset filesize;
    char *buffer;
    MPI_File mpi_fh;
    int ierr;

    // Receive the size of the file from the sender
    MPI_Recv(&filesize, 1, MPI_OFFSET, source_rank, 0, MPI_COMM_WORLD, &status);

    // Allocate a buffer to receive the file
    buffer = (char *)malloc(filesize);
    if (!buffer) {
        perror("Unable to allocate buffer for incoming file");
        return;
    }

    // Receive the file data from the sender
    MPI_Recv(buffer, filesize, MPI_BYTE, source_rank, 0, MPI_COMM_WORLD, &status);

    // Open the file to write in write-only mode
    ierr = MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &mpi_fh);
    if (ierr) {
        fprintf(stderr, "Unable to open file '%s' for writing\n", filename);
        free(buffer);
        return;
    }

    // Write the buffer to the file
    MPI_File_write(mpi_fh, buffer, filesize, MPI_BYTE, &status);

    // Clean up
    free(buffer);
    MPI_File_close(&mpi_fh);
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: mpirun -np 2 %s <filename>\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == SEND_RANK) {
        send_file(argv[1], RECV_RANK);
    } else if (rank == RECV_RANK) {
        receive_file("received_file", SEND_RANK);
    }

    MPI_Finalize();
    return 0;
}