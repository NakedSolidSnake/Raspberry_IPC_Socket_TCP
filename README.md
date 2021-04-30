<p align="center">
  <img src="https://i.ytimg.com/vi/ou8UVN7SG6s/maxresdefault.jpg">
</p>

# _Socket TCP_

## Tópicos
* [Introdução](#introdução)
* [Implementação](#implementação)
* [launch_processes](#launch_processes)
* [button_interface](#button_interface)
* [led_interface](#led_interface)
* [Compilando, Executando e Matando os processos](#compilando-executando-e-matando-os-processos)
* [Compilando](#compilando)
* [Clonando o projeto](#clonando-o-projeto)
* [Selecionando o modo](#selecionando-o-modo)
* [Modo PC](#modo-pc)
* [Modo RASPBERRY](#modo-raspberry)
* [Executando](#executando)
* [Interagindo com o exemplo](#interagindo-com-o-exemplo)
* [MODO PC](#modo-pc-1)
* [MODO RASPBERRY](#modo-raspberry-1)
* [Matando os processos](#matando-os-processos)
* [Conclusão](#conclusão)
* [Referência](#referência)

## Introdução
Até o momento nos referimos a IPC's que permite a comunicação entre processos em uma mesma máquina, o que não é o caso de _socket_. _Sockets_ é um tipo de IPC muito especial, para não dizer o melhor, esse IPC permite a comunicação entre dois processos na mesma máquina, bem como a comunicação entre dois processos em máquinas diferentes atráves de uma rede, ou seja, dois processos rodando em computadores fisicamente separados. Um socket é um dispositivo de comunicação bidirecional, sendo possível enviar e receber mensagens. Para entender de forma fácil o que seria o _socket_, é fazer uma analogia com uma ligação telefônica, você deseja ligar para uma determinado número, então disca esse número e quando a conexão é estabelecida começa a tocar no telefone na outra ponta, a pessoa atende e começam a conversar, o mesmo ocorre para o _socket_, porém ao invés de usar número usamos IP e porta para estabelecer a comunicação. Este IPC possui duas formas de comunicação(não irei mencionar as outras devido não serem tão utilizadas) conhecidas como TCP(Transmission Control Protocol) nesse caso as mensagens são enviadas na forma de _stream_ de mensagens, e UDP(User Datagram Protocol) onde as mensagens são enviadas em blocos de mensagens. Nesse artigo iremos abordar o TCP.


## TCP
O Assunto sobre TCP é imenso, por isso iremos nos limitar somente ao funcionamento desse IPC, ou seja, na sua aplicação, caso queria saber mais como o protocolo funciona, nas referências consta a bibliografia utilizada.
O TCP é considerado um protocolo confiável, pois provê garantia de entrega das mensagens, e de forma ordenada, roda sobre o protocolo IP, e sendo ele um protocolo orientado a conexão, necessita de uma troca de dados iniciais entre os envolvidos para estabelecer uma conexão TCP, conhecido como _handshake_, o cliente envia um SYN para o servidor, então o servidor responde com um SYN ACK e por fim o cliente responde com um ACK.

<p align="center">
  <img src="./img/handshake.gif">
</p>

O TCP permite conexões entre processos em máquinas distintas, dessa forma podemos atribuir funções para cada uma dessas máquinas, caracterizando uma aplicação distribuída, onde cada possui uma responsabilidade dentro da aplicação. A figura abaixo demonstra a conexão entre duas máquinas:

<p align="center">
  <img src="./img/sockets.png">
</p>

Normalmente a arquitetuta mais empregada para esse protocolo é o Cliente/Servidor

## Conceito de servidor
Pela definição do dicionário servidor é um computador que disponibiliza informação e serviços a outros computadores ligados em rede, dessa forma sempre deve estar disponível, para que quando desejado o acesso a ele sempre seja possível.

## Conceito de cliente
Cliente é um computador que consome os serviços e informações de um servidor, podendo ser interno ou pela rede de computadores

## _System Calls utilizados no TCP_ 

Para criar uma aplicação utilizando TCP utilizamos com conjunto bem específico de funções, sendo elas descritas a seguir:


Cria um endpoint para estabelecer uma comunicação
```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

Faz a junção da porta com o socket
```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Entra no modo de escuta, aguardando conexões
```c
#include <sys/types.h>
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

Quando uma conexão é requisitada realiza a aceitação, estabelecendo a conexão
```c
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

```

Solicita uma comunicação
```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Com a conexão estabelecida, permite o envio de mensagens para o endpoint receptor
```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

Com a conexão estabelecida, permite receber mensagens do endpoint emissor
```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

Solicita o fim de recepção de novas mensagens
```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);
```

Destroí o socket
```c
#include <unistd.h>

int close(int fd);
```

## Criando um socket Servidor
Para a criação de uma conexão para servidor é necessário seguir alguns passos:
1. Criar um socket
2. Realizar um bind com a porta especificada
3. Iniciar a escuta de requisições de novas conexões
4. Estabelecer a conexão 
5. Realizar a comunicação entre o servidor e o cliente

## Criando um socket Cliente
Para a criação de uma conexão para cliente é necessário seguir alguns passos:
1. Criar um socket
2. Solicitar a conexão
3. Realizar a comunicação entre o cliente e o servidor

## Destruindo um socket Servidor/Cliente
1. Interromper a troca de mensagens
2. Realizar o fechamento do socket

## Preparação do Ambiente
Antes de apresentarmos o exemplo, primeiro precisaremos instalar algumas ferramentas para auxiliar na análise da comunicação. As ferramentas necessárias para esse artigo são o tcpdump e o netcat(nc), para instalá-los basta executar os comandos abaixo:

```bash
sudo apt-get update
```

```bash
sudo apt-get install netcat
```

```bash
sudo apt-get install tcpdump
```

## netcat
O netcat é uma ferramenta capaz de interagir com conexões UDP e TCP, podendo abrir conexões, e ouvindo como um servidor, ou enviar mensanges para um servidor.

## tcpdump
O tcpdump é uma ferramenta capaz de monitorar o tráfego de dados em uma dada interface como por exemplo eth0, com ele é possível analisar os pacotes que são recebido e enviados.


## Implementação

Para demonstrar o uso desse IPC, iremos utilizar o modelo Cliente/Servidor, onde o processo Cliente(_button_process_) vai enviar uma mensagem com comandos pré-determinados para o servidor, e o Servidor(_led_process_) vai ler as mensagens e verificar se possui o comando cadastrado, assim o executando.
Para melhor isolar as implementações do servidor e do cliente foi criado uma biblioteca, que abstrai a rotina de inicialização e execução do servidor, e a rotina de conexão por parte do cliente.

### Biblioteca
A biblioteca criada permite uma fácil criação do servidor, sendo o servidor orientado a eventos, ou seja, fica aguardando as mensagens chegarem.

#### tcp_interface.h
Primeiramente criamos uma interface resposável por eventos de envio e recebimento, essa funções serão chamadas quando esses eventos ocorrerem.

```c
typedef struct 
{
    int (*on_send)(char *buffer, int *size, void *user_data);  
    int (*on_receive)(char *buffer, int size, void *user_data);
} TCP_Callback_t;
```

#### tcp_server.h

Criamos também um contexto que armazena os paramêtros utilizados pelo servidor, sendo o _socket_ para armazenar a instância criada, _port_ que recebe o número que corresponde onde o serviço será disponibilizado, _buffer_ que aponta para a memória alocada previamente pelo usuário, *buffer_size* o representa o tamanho do _buffer_ e a interface das funções de _callback_

```c
typedef struct
{
    int socket;
    int port;
    char *buffer;
    int buffer_size;
    TCP_Callback_t cb;
} TCP_Server_t;
```

Essa função realiza os passos de 1 a 3 previamente descritos, para a inicilização do servidor
```c
bool TCP_Server_Init(TCP_Server_t *server);
```

Essa função aguarda uma conexão e realiza a comunicação com o cliente.
```c
bool TCP_Server_Exec(TCP_Server_t *server, void *data);
```
#### tcp_server.c

No TCP_Server_Init definimos algumas váriaveis para auxiliar na inicialização do servidor, sendo uma variável booleana que representa o estado da inicialização do servidor, uma variável do tipo inteiro que recebe o resultado das funções necessárias para a configuração, uma variável do tipo inteiro para habilitar o reuso da porta caso o servidor precise reiniciar e um estrutura sockaddr_in que é usada para configurar o servidor para se comunicar através da rede.
```c
bool status = false;
int is_valid;
int enable_reuse = 1;
struct sockaddr_in address;
```
Para realizar a inicialização é criado um dummy do while, para que quando houver falha em qualquer uma das etapas sair da função com status de erro, nesse ponto verificamos se o contexto e o buffer foi inicializado, que é de reponsabilidade do usuário

```c
if(!server || !server->buffer)
    break;
```
Criamos um endpoint com o perfil de se conectar via protocolo IPv4(AF_INET), do tipo stream que caracteriza o TCP(SOCK_STREAM), o último parâmetro pode ser 0 nesse caso.
```c
server->socket = socket(AF_INET, SOCK_STREAM, 0);
if(server->socket < 0)
    break;
```
Aqui permitimos o reuso do socket caso necessite reiniciar o serviço
```c
is_valid = setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (void *)&enable_reuse, sizeof(enable_reuse));
if(is_valid < 0)
    break;
```

Preenchemos a estrutura com parâmetros fornecidos pelo usuário como em qual porta que o serviço vai rodar.
```c
memset(&address, 0, sizeof(address));

address.sin_family = AF_INET;
address.sin_addr.s_addr = htonl(INADDR_ANY);
address.sin_port = htons(server->port);
```

Aplicamos as configurações ao socket criado
```c
is_valid = bind(server->socket, (struct sockaddr *)&address, sizeof(address));
if(is_valid != 0)
    break;
```

Por fim colocamos o socket para escutar novas conexões
```c
is_valid = listen(server->socket, 1);
if(is_valid < 0)
    break;

status = true;
```

#### tcp_client.h
Criamos também um contexto que armazena os paramêtros utilizados pelo cliente, sendo o _socket_ para armazenar a instância criada, _hostname_ é o ip que da máquina que vai ser conectar, _port_ que recebe o número que corresponde qual o serviço deseja consumir, _buffer_ que aponta para a memória alocada previamente pelo usuário, *buffer_size* o representa o tamanho do _buffer_ e a interface das funções de _callback_

```c
typedef struct 
{
    int socket;
    const char *hostname;
    int port;
    char *buffer;
    size_t buffer_size;
    TCP_Callback_t cb;
} TCP_Client_t;
```

Essa função realiza a conexão, envio e recebimento de mensagens para o servidor configurado
```c
bool TCP_Client_Connect(TCP_Client_t *client, void *data);
```

#### tcp_client.c

```c
bool status = false;
int is_valid;
struct sockaddr_in server;
int send_size;
int recv_size;
```

```c
if(!client || !client->buffer || client->buffer_size <= 0)
    break;
```
```c
client->socket = socket(AF_INET, SOCK_STREAM, 0);
if(client->socket < 0)
    break;

```
```c
server.sin_family = AF_INET;
server.sin_port = htons(client->port);
```
```c
is_valid = inet_pton(AF_INET, client->hostname, &server.sin_addr);
if(is_valid <= 0)
    break;
```

```c
is_valid = connect(client->socket, (struct sockaddr *)&server, sizeof(server));
if(is_valid < 0)
    break;

status = true;
```

```c
if( status && client->cb.on_send)
```
```c
client->cb.on_send(client->buffer, &send_size, data);
send(client->socket, client->buffer, (int)fmin(send_size, client->buffer_size), 0);
```
```c
if(client->cb.on_receive)
{
    recv_size = recv(client->socket, client->buffer, client->buffer_size, 0);
    client->cb.on_receive(client->buffer, recv_size, data);
}
```
```c
shutdown(client->socket, SHUT_RDWR);
close(client->socket);

return status;
```






 A aplicação é composta por três executáveis sendo eles:
* _launch_processes_ - é responsável por lançar os processos _button_process_ e _led_process_ atráves da combinação _fork_ e _exec_
* _button_interface_ - é reponsável por ler o GPIO em modo de leitura da Raspberry Pi e se conectar ao servidor para enviar uma mensagem de alteração de estado.
* _led_interface_ - é reponsável por escutar novas conexões, recebendo comandos para aplicar em um GPIO configurado como saída

### *launch_processes*

No _main_ criamos duas variáveis para armazenar o PID do *button_process* e do *led_process*, e mais duas variáveis para armazenar o resultado caso o _exec_ venha a falhar.
```c
int pid_button, pid_led;
int button_status, led_status;
```

Em seguida criamos um processo clone, se processo clone for igual a 0, criamos um _array_ de *strings* com o nome do programa que será usado pelo _exec_, em caso o _exec_ retorne, o estado do retorno é capturado e será impresso no *stdout* e aborta a aplicação. Se o _exec_ for executado com sucesso o programa *button_process* será carregado. 
```c
pid_button = fork();

if(pid_button == 0)
{
    //start button process
    char *args[] = {"./button_process", NULL};
    button_status = execvp(args[0], args);
    printf("Error to start button process, status = %d\n", button_status);
    abort();
}   
```

O mesmo procedimento é repetido novamente, porém com a intenção de carregar o *led_process*.

```c
pid_led = fork();

if(pid_led == 0)
{
    //Start led process
    char *args[] = {"./led_process", NULL};
    led_status = execvp(args[0], args);
    printf("Error to start led process, status = %d\n", led_status);
    abort();
}
```

### *button_interface*
descrever o código
### *led_interface*
descrever o código

## Compilando, Executando e Matando os processos
Para compilar e testar o projeto é necessário instalar a biblioteca de [hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware) necessária para resolver as dependências de configuração de GPIO da Raspberry Pi.

## Compilando
Para faciliar a execução do exemplo, o exemplo proposto foi criado baseado em uma interface, onde é possível selecionar se usará o hardware da Raspberry Pi 3, ou se a interação com o exemplo vai ser através de input feito por FIFO e o output visualizado através de LOG.

### Clonando o projeto
Pra obter uma cópia do projeto execute os comandos a seguir:

```bash
$ git clone https://github.com/NakedSolidSnake/Raspberry_IPC_Socket_TCP
$ cd Raspberry_IPC_Socket_TCP
$ mkdir build && cd build
```

### Selecionando o modo
Para selecionar o modo devemos passar para o cmake uma variável de ambiente chamada de ARCH, e pode-se passar os seguintes valores, PC ou RASPBERRY, para o caso de PC o exemplo terá sua interface preenchida com os sources presentes na pasta src/platform/pc, que permite a interação com o exemplo através de FIFO e LOG, caso seja RASPBERRY usará os GPIO's descritos no [artigo](https://github.com/NakedSolidSnake/Raspberry_lib_hardware#testando-a-instala%C3%A7%C3%A3o-e-as-conex%C3%B5es-de-hardware).

#### Modo PC
```bash
$ cmake -DARCH=PC ..
$ make
```

#### Modo RASPBERRY
```bash
$ cmake -DARCH=RASPBERRY ..
$ make
```

## Executando
Para executar a aplicação execute o processo _*launch_processes*_ para lançar os processos *button_process* e *led_process* que foram determinados de acordo com o modo selecionado.

```bash
$ cd bin
$ ./launch_processes
```

Uma vez executado podemos verificar se os processos estão rodando atráves do comando 
```bash
$ ps -ef | grep _process
```

O output 
```bash
pi        2773     1  0 10:25 pts/0    00:00:00 led_process
pi        2774     1  1 10:25 pts/0    00:00:00 button_process 2773
```
## Interagindo com o exemplo
Dependendo do modo de compilação selecionado a interação com o exemplo acontece de forma diferente

### MODO PC
Para o modo PC, precisamos abrir um terminal e monitorar os LOG's
```bash
$ sudo tail -f /var/log/syslog | grep LED
```

Dessa forma o terminal irá apresentar somente os LOG's referente ao exemplo.

Para simular o botão, o processo em modo PC cria uma FIFO para permitir enviar comandos para a aplicação, dessa forma todas as vezes que for enviado o número 0 irá logar no terminal onde foi configurado para o monitoramento, segue o exemplo
```bash
echo "0" > /tmp/tcp_file
```

Output do LOG quando enviado o comando algumas vezez
```bash
Apr  6 06:22:37 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: On
Apr  6 06:22:39 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: Off
Apr  6 06:22:40 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: On
Apr  6 06:22:40 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: Off
Apr  6 06:22:41 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: On
Apr  6 06:22:42 cssouza-Latitude-5490 LED SIGNAL[4277]: LED Status: Off
```

### MODO RASPBERRY
Para o modo RASPBERRY a cada vez que o botão for pressionado irá alternar o estado do LED.

## Matando os processos
Para matar os processos criados execute o script kill_process.sh
```bash
$ cd bin
$ ./kill_process.sh
```

## Conclusão
Preencher

## Referência
* [Link do projeto completo](https://github.com/NakedSolidSnake/Raspberry_IPC_Socket_TCP)
* [Mark Mitchell, Jeffrey Oldham, and Alex Samuel - Advanced Linux Programming](https://www.amazon.com.br/Advanced-Linux-Programming-CodeSourcery-LLC/dp/0735710430)
* [fork, exec e daemon](https://github.com/NakedSolidSnake/Raspberry_fork_exec_daemon)
* [biblioteca hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware)

