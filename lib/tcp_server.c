#include <tcp_server.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

bool TCP_Server_Init(TCP_Server_t *server)
{
    bool status = false;
    int is_valid;
    int enable_reuse = 1;
    struct sockaddr_in address;

    do 
    {
        if(!server || !server->buffer)
            break;

        server->socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server->socket < 0)
            break;

        is_valid = setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (void *)&enable_reuse, sizeof(enable_reuse));
        if(is_valid < 0)
            break;

        memset(&address, 0, sizeof(address));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(server->port);        

        is_valid = bind(server->socket, (struct sockaddr *)&address, sizeof(address));
        if(is_valid != 0)
            break;

        is_valid = listen(server->socket, 1);
        if(is_valid < 0)
            break;

        status = true;

    }while(false);

    return status;
}

bool TCP_Server_Exec(TCP_Server_t *server, void *data)
{
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    int client_socket;
    size_t read_len;
    int write_len;    
    bool status = false;
  

    client_socket = accept(server->socket, (struct sockaddr *)&address, &addr_len);
    if(client_socket > 0)
    {
        read_len = recv(client_socket, server->buffer, server->buffer_size, 0);
        if(server->cb.on_receive)
        {
            server->cb.on_receive(server->buffer, read_len, data);
        }

        if(server->cb.on_send)
        {
            server->cb.on_send(server->buffer, &write_len, data);
            send(client_socket, server->buffer, (int)fmin(write_len, server->buffer_size), 0);
        }

        status = true;

        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
    }
        
    return status;    
}