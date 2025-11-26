#ifndef DISPLAY_DUMMY_H
#define DISPLAY_DUMMY_H

#include <stdint.h>
#include <stdbool.h>
#include "controller.h"

void display_dummy_init(void);
void display_dummy_show(float vel_kmh, tipo_veiculo_t tipo, status_t status);
void display_dummy_thread(void *, void *, void *);

#endif
