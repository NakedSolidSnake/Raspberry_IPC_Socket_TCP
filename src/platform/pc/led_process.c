#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <syslog.h>
#include <led_interface.h>

#define BUFFER_SIZE 1024

static int on_receive_message(char *buffer, int size, void *user_data);

bool Init(void *object);
bool Set(void *object, uint8_t state);

int main(int argc, char *argv[])
{   
    char server_buffer[BUFFER_SIZE];

    LED_Interface led_interface = 
    {
        .Init = Init,
        .Set = Set
    };

    LED_Data data = 
    {
        .object = NULL,
        .interface = &led_interface
    };

    TCP_Server_t server = 
    {
        .port = 5555,
        .buffer = server_buffer,
        .buffer_size = sizeof(server_buffer),
        .cb.on_receive = on_receive_message
    };

    LED_Run(&server, &data);

    return 0;
}

bool Init(void *object)
{
    (void)object; 
    return true;
}

bool Set(void *object, uint8_t state)
{
    (void)object;    
    openlog("LED PIPE", LOG_PID | LOG_CONS , LOG_USER);
    syslog(LOG_INFO, "LED Status: %s", state ? "On": "Off");
    closelog(); 
    return true;
}

static int on_receive_message(char *buffer, int size, void *user_data)
{
    LED_Data *led = (LED_Data *)user_data;

    if(strncmp("LED ON", buffer, strlen("LED ON")) == 0)
        led->interface->Set(led->object, 1);
    else if(strncmp("LED OFF", buffer, strlen("LED OFF")) == 0)
        led->interface->Set(led->object, 0);

    return 0;
}
