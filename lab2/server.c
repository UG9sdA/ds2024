#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "file_transfer.h"

#define MAX_BUFFER 1024

FILE_TRANSFER_PROC_RES *file_transfer_proc_1_svc(char **filename, struct svc_req *rqstp) {
    static FILE_TRANSFER_PROC_RES result;
    FILE *file = fopen(*filename, "rb");
    if (file == NULL) {
        result.error = 1;
        return &result;
    }

    int bytesRead;
    result.error = 0;
    result.FILE_TRANSFER_PROC_RES_u.data.data_val = (char *)malloc(MAX_BUFFER);
    while ((bytesRead = fread(result.FILE_TRANSFER_PROC_RES_u.data.data_val, 1, MAX_BUFFER, file)) > 0) {
        result.FILE_TRANSFER_PROC_RES_u.data.data_len = bytesRead;
        if (!file_transfer_proc_res_1(&result, rqstp)) {
            fprintf(stderr, "Error: RPC call failed\n");
            exit(1);
        }
    }

    fclose(file);
    return &result;
}

int main() {
    if (freopen("rpclog", "w", stderr) == NULL) {
        fprintf(stderr, "Cannot open rpclog\n");
        exit(1);
    }

    if (svc_create(file_transfer_prog, FILE_TRANSFER_VERS, FILE_TRANSFER_VERS, "tcp") == NULL) {
        fprintf(stderr, "Error: Failed to register the FILE_TRANSFER service\n");
        exit(1);
    }

    svc_run();
    fprintf(stderr, "Error: svc_run returned unexpectedly\n");
    exit(1);
}