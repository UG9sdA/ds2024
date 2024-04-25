#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "file_transfer.h"

#define MAX_BUFFER 1024

int main() {
    CLIENT *client;
    FILE_TRANSFER_PROC_RES *result;
    char *server = "localhost";
    char *filename = "file.txt";

    client = clnt_create(server, FILE_TRANSFER_PROG, FILE_TRANSFER_VERS, "tcp");
    if (client == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    result = file_transfer_proc_1(&filename, client);
    if (result == NULL) {
        clnt_perror(client, server);
        exit(1);
    }

    if (result->error) {
        fprintf(stderr, "Error: Failed to open file\n");
        exit(1);
    }

    FILE *file = fopen("received_file.txt", "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to create file\n");
        exit(1);
    }

    fwrite(result->FILE_TRANSFER_PROC_RES_u.data.data_val, 1, result->FILE_TRANSFER_PROC_RES_u.data.data_len, file);
    fclose(file);

    clnt_destroy(client);
    exit(0);
}