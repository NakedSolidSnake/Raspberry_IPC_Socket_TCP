#include <server.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define ISVALIDSOCKET(s) ((s) >= 0)

int Server_init(Server_t *server)
{
    struct addrinfo hints;

    if(!server)
        return -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, server->port, &hints, &bind_address);

    server->socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(server->socket)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    int yes = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0) {
        fprintf(stderr, "setsockopt() failed. (%d)\n", errno);
    }
    
    if (bind(server->socket, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);
    
    if (listen(server->socket, LISTEN_AMOUNT) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    return 0;
}

int Server_exec(Server_t *server)
{  

    fd_set master;
    FD_ZERO(&master);
    FD_SET(server->socket, &master);
    int max_socket = server->socket;

    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return 1;
        }

        int i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {

                if (i == server->socket) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    int socket_client = accept(server->socket, (struct sockaddr*) &client_address, &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                errno);
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);                    

                } else {
                    char buffer[MAX_BUFFER_SEND_RECV] = {0};
                    int bytes_received = recv(i, buffer, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        close(i);                        
                        continue;
                    }

                    //Server process
                    server->cb.recv(buffer, bytes_received);

                    if(server->cb.send){
                        server->cb.send(buffer, &bytes_received);
                        send(i, buffer, bytes_received, 0);
                    }

                }

            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)

    close(server->socket);
    return 0;
}
