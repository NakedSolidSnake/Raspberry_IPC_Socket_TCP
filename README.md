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
Até o momento nos referimos a IPC's que permite a comunicação entre processos em uma mesma máquina, o que não é o caso de Socket. Sockets são um tipo de IPC muito especial, para não dizer o melhor, esse IPC permite a comunicação entre dois processos na mesma máquina, bem como a comunicação entre dois processos em máquinas diferentes atráves de uma rede, ou seja, dois processos rodando em computadores fisicamente separados. Um socket é um dispositivo de comunicação bidirecional, é possível tando enviar mensagens ou receber mensagens. Este IPC possui duas formas de comunicação(não irei mencionar as outras devido não serem tão utilizadas) conhecidas como TCP(Transmission Control Protocol) e UDP(User Datagram Protocol). Nesse artigo iremos abordar o TCP.


## TCP
O Assunto sobre TCP é imenso, por isso iremos nos limitar somente ao funcionamento desse IPC, ou seja, na sua aplicação, caso queria saber mais como o protocolo funciona, nas referências consta a bibliografia utilizada.
O TCP é considerado um protocolo confiável, pois provê garantia de entrega das mensagens, e de forma ordenada, sendo ele um protocolo orientado a conexão, 
necessita de uma troca de dados iniciais entre os envolvidos para estabelecer uma conexão TCP, conhecido como _handshake_, o cliente envia um SYN para o servidor, então o servidor responde com um SYN ACK e por fim o cliente responde com um ACK.

<p align="center">
  <img src="./img/handshake.gif">
</p>

O TCP permite conexões entre processos em máquinas distintas, dessa forma podemos atribuir funções para cada uma dessas máquinas, caracterizando uma aplicação distribuída, onde cada uma dessas máquinas possui uma responsabilidade dentro da aplicação. A figura abaixo demonstra a conexão entre duas máquinas:

<p align="center">
  <img src="./img/sockets.png">
</p>


## _System Calls utilizados no TCP_ 

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

```c
#include <sys/types.h>
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

```c
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

```

```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);
```

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
2. Relizar o fechamento do socket

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

Para demonstrar o uso desse IPC, iremos utilizar o modelo Cliente/Servidor, onde o processo Cliente(_button_process_) vai enviar uma mensagem com comandos pré-determinados, e o Servidor(_led_process_) vai ler as mensagens e verificar se possui o comando cadastrado, e vai executá-lo. Aplicação é composta por três executáveis sendo eles:
* _launch_processes_ - é responsável por lançar os processos _button_process_ e _led_process_ atráves da combinação _fork_ e _exec_
* _button_interface_ - é reponsável por ler o GPIO em modo de leitura da Raspberry Pi e escrever o estado interno no arquivo
* _led_interface_ - é reponsável por ler do arquivo o estado interno do botão e aplicar em um GPIO configurado como saída

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

## *button_interface*
descrever o código
## *led_interface*
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
echo "0" > /tmp/signal_file
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

