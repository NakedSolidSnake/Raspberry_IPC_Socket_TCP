#include <tcp_client.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


bool TCP_Client_Connect(TCP_Client_t *client, void *data)
{
    bool status = false;
    int is_valid;
    struct sockaddr_in server;
    int send_size;
    int recv_size;

    do 
    {

        if(!client || !client->buffer || client->buffer_size <= 0)
            break;

        client->socket = socket(AF_INET, SOCK_STREAM, 0);
        if(client->socket < 0)
            break;

        server.sin_family = AF_INET;
        server.sin_port = htons(client->port);

        is_valid = inet_pton(AF_INET, client->hostname, &server.sin_addr);
        if(is_valid <= 0)
            break;

        is_valid = connect(client->socket, (struct sockaddr *)&server, sizeof(server));
        if(is_valid < 0)
            break;

        status = true;

    } while(false);
    
    if( status && client->cb.on_send)
    {
        client->cb.on_send(client->buffer, &send_size, data);
        send(client->socket, client->buffer, (int)fmin(send_size, client->buffer_size), 0);

        if(client->cb.on_receive)
        {
            recv_size = recv(client->socket, client->buffer, client->buffer_size, 0);
            client->cb.on_receive(client->buffer, recv_size, data);
        }
    }

    shutdown(client->socket, SHUT_RDWR);
    close(client->socket);    

    return false;
}