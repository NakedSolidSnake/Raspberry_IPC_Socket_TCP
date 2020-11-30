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

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <client.h>
#include <button.h>

#define _1ms    1000

static int send(char *buffer, int size);

 const char *states[] = 
    {
        "ON",
        "OFF"
    };

static Button_t button7 = {
    .gpio.pin = 7,
    .gpio.eMode = eModeInput,
    .ePullMode = ePullModePullUp,
    .eIntEdge = eIntEdgeFalling,
    .cb = NULL};

int main(int argc, char *argv[])
{

    Client_t cl =
        {
            .socket = -1,
            .console = CONSOLE_ENABLE,
            .cb.send = send,
            .cb.receive = NULL};

    if (Button_init(&button7))
        return EXIT_FAILURE;

    Client_connect(&cl, "192.168.0.25", "8080");
    Client_exec(&cl);
}

static int send(char *buffer, int size)
{
    static int state = 0;
    while (1)
    {
        if (!Button_read(&button7))
        {
            usleep(_1ms * 40);
            while (!Button_read(&button7))
                ;
            usleep(_1ms * 40);
            state ^= 0x01;
            snprintf(buffer, strlen(states[state]) + 1, "%s",states[state]);
            break;
        }
        else
        {
            usleep(_1ms);
        }
    }
}
