#include <stdio.h>
#include <stdlib.h>
#include "../include/server.h"

// Function declaration
int start_server();

int main()
{
    if (start_server() < 0)
    {
        fprintf(stderr, "Failed to start server\n");
        return 0;
    }
    return 1;
}
