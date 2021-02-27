<p align="center">
  <img src="https://i.ytimg.com/vi/ou8UVN7SG6s/maxresdefault.jpg">
</p>

# _Socket_
## Introdução
## Implementação
### biblioteca
A implementação de um socket é um pouco extensa para melhorar a compreensão da implementação, a parte de pertinente a controle do servidor foi abstraída
#### client.h
```c
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
```
#### client.c
```c
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
```
#### server.h
```c
/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __SERVER_H
#define __SERVER_H

#define MAX_PORT_LEN             6
#define MAX_BUFFER_SEND_RECV     1024
#define LISTEN_AMOUNT            10


/**
 * @brief Estrutura de callbacks para interagir com o server
 * 
 */
typedef struct 
{
    int (*send)(char *buffer, int *size);   /**<! Callback para enviar dados pelo server */
    int (*recv)(char *buffer, int size);    /**<! Callback para receber dados do server */
}Server_Callback_t;


/**
 * @brief Estrutura de contexto do server
 * 
 */
typedef struct
{
    int socket;                 /**<! File descriptor do servidor */
    char port[MAX_PORT_LEN];    /**<! Porta do servidor */
    Server_Callback_t cb;       /**<! Callbacks */
}Server_t;


/**
 * @brief 
 * 
 * @param server Contexto do servidor
 * @return int  -1 quando contexto é invalido
 *               1 quando problema ao configurar o servidor
 *               0 sucesso
 */
int Server_init(Server_t *server);

/**
 * @brief Executa server com os parametros previamente iniciados em Server_init
 * 
 * @param server Contexto do servidor
 * @return int   1 em caso de falha
 *               0 em caso de sucesso
 */
int Server_exec(Server_t *server);

#endif
```
#### server.c
```c
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

```
### launch_processes.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid_button, pid_led;
    int button_status, led_status;

    pid_button = fork();

    if(pid_button == 0)
    {
        //start button process
        char *args[] = {"./button_process", NULL};
        button_status = execvp(args[0], args);
        printf("Error to start button process, status = %d\n", button_status);
        abort();
    }   

    pid_led = fork();

    if(pid_led == 0)
    {
        //Start led process
        char *args[] = {"./led_process", NULL};
        led_status = execvp(args[0], args);
        printf("Error to start led process, status = %d\n", led_status);
        abort();
    }

    return EXIT_SUCCESS;
}
```

### button_process.c
```c
/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <client.h>
#include <button.h>

#define _1ms    1000

static int send(char *buffer, int size);

 const char *states[] = 
    {
        "ON",
        "OFF"
    };

static Button_t button = {
    .gpio.pin = 7,
    .gpio.eMode = eModeInput,
    .ePullMode = ePullModePullUp,
    .eIntEdge = eIntEdgeFalling,
    .cb = NULL};

int main(int argc, char *argv[])
{

    Client_t cl =
        {
            .socket = -1,
            .console = CONSOLE_ENABLE,
            .cb.send = send,
            .cb.receive = NULL};

    if (Button_init(&button))
        return EXIT_FAILURE;

    Client_connect(&cl, "192.168.0.25", "8080");
    Client_exec(&cl);
}

static int send(char *buffer, int size)
{
    static int state = 0;
    while (1)
    {
        if (!Button_read(&button))
        {
            usleep(_1ms * 40);
            while (!Button_read(&button))
                ;
            usleep(_1ms * 40);
            state ^= 0x01;
            snprintf(buffer, strlen(states[state]) + 1, "%s",states[state]);
            break;
        }
        else
        {
            usleep(_1ms);
        }
    }
}

```

### led_process.c
```c
#include <server.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <led.h>

typedef void (*cb)(int led_state);

static int sender(char *buffer, int *size);
static int receive(char *buffer, int size);

static void changeLed(int state);

typedef struct
{
    const char *command;
    cb setLed;
    const int state;
} Table_t;

static Table_t command[] =
    {
        {"ON", changeLed, 1},
        {"OFF", changeLed, 0}};

LED_t led =
    {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput};

int main()
{
    Server_t s =
        {
            .socket = -1,
            .port = "8080",
            .cb.recv = receive,
            // .cb.send = sender
	};

    if (LED_init(&led))
        return EXIT_FAILURE;

    Server_init(&s);
    Server_exec(&s);

    return 0;
}

static int sender(char *buffer, int *size)
{
    return 0;
}

static int receive(char *buffer, int size)
{
    printf("%s", buffer);
    buffer[size - 1] = '\0';
    for (int i = 0; i < (sizeof(command) / sizeof(command[0])); i++)
    {
        if (!strncmp(buffer, command[i].command, size))
        {
            command[i].setLed(command[i].state);
            break;
        }
    }

    return 0;
}

static void changeLed(int state)
{
    LED_set(&led, (eState_t)state);
}

```

## Conclusão
