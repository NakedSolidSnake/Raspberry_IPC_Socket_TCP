#include <client.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ISVALIDSOCKET(s) ((s) >= 0)

int Client_connect(Client_t *cl, const char *server_ip, const char *port)
{

    if (!cl)
    {
        return EXIT_FAILURE;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(server_ip, port, &hints, &peer_address))
    {
        return 1;
    }

    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);

    cl->socket = socket(peer_address->ai_family,
                        peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(cl->socket))
    {
        return 1;
    }

    if (connect(cl->socket,
                peer_address->ai_addr, peer_address->ai_addrlen))
    {
        return 1;
    }
    freeaddrinfo(peer_address);

    return EXIT_SUCCESS;
}

int Client_exec(Client_t *cl)
{
    char buffer[BUFFER_RECV_SEND] = {0};
    while (1)
    {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(cl->socket, &reads);
        FD_SET(0, &reads);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (cl->cb.send)
        {
            cl->cb.send(buffer, BUFFER_RECV_SEND);
            send(cl->socket, buffer, strlen(buffer) + 1, 0);
        }

        if (select(cl->socket + 1, &reads, 0, 0, &timeout) < 0)
        {
            return 1;
        }

        if (FD_ISSET(cl->socket, &reads))
        {            
            int bytes_received = recv(cl->socket, buffer, BUFFER_RECV_SEND, 0);
            if (cl->cb.receive)
            {
                cl->cb.receive(buffer, sizeof(buffer));
            }
            if (bytes_received < 1)
            {
                break;
            }
        }

        if (cl->console == CONSOLE_DISABLE)
            continue;

        if (FD_ISSET(0, &reads))
        {
            char readLine[BUFFER_CONSOLE];
            if (!fgets(readLine, BUFFER_CONSOLE, stdin))
                break;

            send(cl->socket, readLine, strlen(buffer), 0);
        }
    } //end while(1)

    close(cl->socket);

    return EXIT_SUCCESS;
}