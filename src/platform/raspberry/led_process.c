#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <led.h>
#include <led_interface.h>

#define BUFFER_SIZE 1024

static int on_receive_message(char *buffer, int size, void *user_data);

bool Init(void *object);
bool Set(void *object, uint8_t state);

int main(int argc, char *argv[])
{   
    char server_buffer[BUFFER_SIZE];
    
    LED_t led =
    {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

    LED_Interface led_interface = 
    {
        .Init = Init,
        .Set = Set
    };

    LED_Data data = 
    {
        .object = &led,
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
    LED_t *led = (LED_t *)object; 
    return LED_init(led) == EXIT_SUCCESS ? true : false;   
}

bool Set(void *object, uint8_t state)
{
    LED_t *led = (LED_t *)object;
    return LED_set(led, (eState_t)state) == EXIT_SUCCESS ? true : false;
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
