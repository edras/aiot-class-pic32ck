/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    mqtt_topics_hal.h

  @Summary
    Platform specific mqtt topic definitions.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _MQTT_TOPICS_HAL_H    /* Guard against multiple inclusion */
#define _MQTT_TOPICS_HAL_H

#include "hal.h"

void MQTT_init_topics_hal(void);

/*
 * prototype APP topics
 * These functions are called directly from app
 * normal publish functions (polled) are defined on the TopicItem structure
 */

void MQTT_publish_capsens(uint8_t button, uint8_t status);
void MQTT_publish_ble_text(char *text_ble);

#endif /* _MQTT_TOPICS_HAL_H */

/* *****************************************************************************
 End of File
 */
