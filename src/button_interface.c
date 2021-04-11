#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <button_interface.h>

#define _1ms    1000

bool Button_Run(TCP_Client_t *client, Button_Data *button)
{
    static int state = 0;

    if(button->interface->Init(button->object) == false)
        return false;

    while(true)
    {
        while (true)
        {
            if (!button->interface->Read(button->object))
            {
                usleep(_1ms * 100);
                state ^= 0x01;
                break;
            }
            else
            {
                usleep(_1ms);
            }
        }

        TCP_Client_Connect(client, &state);
    }
}