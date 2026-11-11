#include "t1s_lwip_hal.h"

#ifdef MIKROE_T1S

void T1S_EIC_CB_Register(void (* callback)(uintptr_t context), uintptr_t context)
{
    EIC_CallbackRegister(EIC_PIN_12, callback, context);
}

#else

#include "lwip/apps/mqtt.h"
void T1S_execute(void) { }
err_t mqtt_sub_unsub(mqtt_client_t *client, const char *topic, u8_t qos, mqtt_request_cb_t cb, void *arg, u8_t sub) 
{ return ERR_OK; }
void mqtt_disconnect(mqtt_client_t *client){}
mqtt_client_t *mqtt_client_new(void) { return NULL; }
err_t mqtt_publish(mqtt_client_t *client, const char *topic, const void *payload, 
        u16_t payload_length, u8_t qos, u8_t retain, mqtt_request_cb_t cb, void *arg)
{ return ERR_OK; }
err_t mqtt_client_connect(mqtt_client_t *client, const ip_addr_t *ipaddr, u16_t port, 
        mqtt_connection_cb_t cb, void *arg, const struct mqtt_connect_client_info_t *client_info)
{ return ERR_OK; }
void mqtt_set_inpub_callback(mqtt_client_t *client, mqtt_incoming_publish_cb_t cb,
                             mqtt_incoming_data_cb_t data_cb, void *arg)
{}


bool T1S_init(void)
{
    return false;
}

bool T1S_available(void)
{
    return false;
}

void T1S_set_debug(bool state)
{
    
}

void T1S_set_bridge(bool state)
{
    
}

void T1S_print_menu(void)
{
    
}

void T1S_send_cmd(char *buffer)
{
    
}

#endif

