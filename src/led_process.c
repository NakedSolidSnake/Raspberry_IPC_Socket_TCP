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
