#ifndef VEHICLE_SIMULATOR_H
#define VEHICLE_SIMULATOR_H

#include <stdint.h>


void vehicle_simulator_start(void);
void vehicle_simulator_stop(void);

const char* vehicle_simulator_generate(void);

void vehicle_simulator_press_enter(void);

#endif 