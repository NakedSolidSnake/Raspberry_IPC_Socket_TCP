#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <led_interface.h>


bool LED_Run(TCP_Server_t *server, LED_Data *led)
{
    if(led->interface->Init(led->object) == false)
        return false;

    TCP_Server_Init(server);

    while(true)
    {
        TCP_Server_Exec(server, led);
    }

    return false;
}
