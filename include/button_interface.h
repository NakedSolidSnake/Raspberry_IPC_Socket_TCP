#ifndef BUTTON_INTERFACE_H_
#define BUTTON_INTERFACE_H_

#include <stdbool.h>
#include <tcp_client.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Read)(void *object);
    
} Button_Interface;

typedef struct 
{
    void *object;
    Button_Interface *interface;
} Button_Data;

bool Button_Run(TCP_Client_t *client, Button_Data *button);

#endif /* BUTTON_INTERFACE_H_ */
