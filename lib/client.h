#ifndef __CLIENT_H
#define __CLIENT_H

#define CONSOLE_DISABLE         0
#define CONSOLE_ENABLE          1

#define BUFFER_RECV_SEND        4092
#define BUFFER_CONSOLE          4092


typedef struct 
{
    int (*receive)(void *buffer, int size);
    int (*send)(void *buffer, int size);
}Callback_t;

typedef struct 
{
    int socket;
    int console;
    Callback_t cb;
}Client_t;

int Client_connect(Client_t *cl, const char *server_ip, const char *port);
int Client_exec(Client_t *cl);

#endif