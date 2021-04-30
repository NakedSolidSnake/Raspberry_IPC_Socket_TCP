#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <tcp_interface.h>

/**
 * @brief Estrutura de contexto do server
 * 
 */
typedef struct
{
    int socket;
    int port;
    char *buffer;
    int buffer_size;
    TCP_Callback_t cb;
} TCP_Server_t;



bool TCP_Server_Init(TCP_Server_t *server);

bool TCP_Server_Exec(TCP_Server_t *server, void *data);

#endif /* TCP_SERVER_H_ */
