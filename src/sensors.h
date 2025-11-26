#ifndef SENSORS_H
#define SENSORS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <stdint.h>

struct sensor_msg {
    uint8_t num_eixos;
    uint32_t tempo_ms;
};

void sensors_init(void);
void sensors_thread(void *a, void *b, void *c);

#endif 
