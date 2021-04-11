#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <button.h>
#include <button_interface.h>

#define BUFFER_SIZE 1024

static bool Init(void *object);
static bool Read(void *object);

static int on_send(char *buffer, int *size, void *data);

 const char *states[] = 
{
    "LED ON",
    "LED OFF"
};

int main(int argc, char *argv[])
{
    char client_buffer[BUFFER_SIZE];

    Button_t btn = {
        .gpio.pin = 7,
        .gpio.eMode = eModeInput,
        .ePullMode = ePullModePullUp,
        .eIntEdge = eIntEdgeFalling,
        .cb = NULL
    };

    Button_Interface button_interface = 
    {
        .Init = Init,
        .Read = Read
    };

    Button_Data button = 
    {
        .object = &btn, 
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
    Button_t *button = (Button_t *)object;
    return Button_init(button) == EXIT_SUCCESS ? true : false;
}

static bool Read(void *object)
{
    Button_t *button = (Button_t *)object;
    return (bool)Button_read(button);
}

static int on_send(char *buffer, int *size, void *data)
{
    int *state = (int *)data;
    snprintf(buffer, strlen(states[*state]) + 1, "%s",states[*state]);
    *size = strlen(states[*state]) + 1;

    return 0;
}