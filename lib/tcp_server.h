#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Estrutura de callbacks para interagir com o server
 * 
 */
typedef struct 
{
    int (*on_send)(char *buffer, int *size, void *user_data);   /**<! Callback para enviar dados pelo server */
    int (*on_receive)(char *buffer, int size, void *user_data);    /**<! Callback para receber dados do server */
} TCP_Server_Callback_t;


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
    TCP_Server_Callback_t cb;
} TCP_Server_t;



bool TCP_Server_Init(TCP_Server_t *server);

bool TCP_Server_Exec(TCP_Server_t *server, void *data);

#endif /* TCP_SERVER_H_ */
