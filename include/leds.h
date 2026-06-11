/**
 * @brief   Estructura y prototipo de la tarea de control de LEDs
 * @author  Ricardo Bárcenas, Daniela Valle
 */

 
#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

typedef struct
{
    uint8_t gpio;
    uint8_t bit_position;
    uint8_t reserved[2]; // Relleno para alinear a 4 bytes

    
} LedTaskParams_t;

void led_task(void *pvParameters);

#endif