#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct 
{
    int (*on_receive)(void *buffer, int size, void *user_data);
    int (*on_send)(char *buffer, int *size, void *user_data);
} TCP_Callback_t;

typedef struct 
{
    int socket;
    const char *hostname;
    int port;
    char *buffer;
    size_t buffer_size;
    TCP_Callback_t cb;
} TCP_Client_t;

bool TCP_Client_Connect(TCP_Client_t *client, void *data);

#endif /* TCP_CLIENT_H_ */
