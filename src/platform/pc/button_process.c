#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <button_interface.h>
#include <sys/stat.h> 
#include <sys/types.h>

#define BUFFER_SIZE 1024


static bool Init(void *object);
static bool Read(void *object);
static const char * myfifo = "/tmp/tcp_file";

static int fd;

static int on_send(char *buffer, int *size, void *data);

 const char *states[] = 
{
    "LED ON",
    "LED OFF"
};

int main(int argc, char *argv[])
{    
    char client_buffer[BUFFER_SIZE];

    Button_Interface button_interface = 
    {
        .Init = Init,
        .Read = Read
    };

    Button_Data button = 
    {
        .object = NULL, 
        .interface = &button_interface
    };

    TCP_Client_t client = 
    {
        .buffer = client_buffer,
        .buffer_size = BUFFER_SIZE,
        .hostname = "127.0.0.1",
        .port = 5555,
        .cb.on_send = on_send        
    };

    Button_Run(&client, &button);
        
    return 0;
}

static bool Init(void *object)
{    
    (void)object;
    remove(myfifo);
    int ret = mkfifo(myfifo, 0666);
    return (ret == -1 ? false : true);
}

static bool Read(void *object)
{
    (void)object;
    int state;
    char buffer[2];

    fd = open(myfifo,O_RDONLY);
    read(fd, buffer, 2);	
    state = atoi(buffer);
    return state ? true : false;
}

static int on_send(char *buffer, int *size, void *data)
{
    int *state = (int *)data;
    snprintf(buffer, strlen(states[*state]) + 1, "%s",states[*state]);
    *size = strlen(states[*state]) + 1;

    return 0;
}