#ifndef TCP_INTERFACE_H
#define TCP_INTERFACE_H

typedef struct 
{
    int (*on_send)(char *buffer, int *size, void *user_data);  
    int (*on_receive)(char *buffer, int size, void *user_data);
} TCP_Callback_t;

#endif /* TCP_INTERFACE_H */
