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