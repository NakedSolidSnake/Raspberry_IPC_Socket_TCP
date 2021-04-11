#ifndef LED_INTERFACE_H_
#define LED_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>
#include <tcp_server.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Set)(void *object, uint8_t state);
} LED_Interface;

typedef struct 
{
    void *object;
    LED_Interface *interface;
} LED_Data;


bool LED_Run(TCP_Server_t *server, LED_Data *led);

#endif /* LED_INTERFACE_H_ */
