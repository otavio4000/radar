// Arquivo: display_dummy.h

#ifndef DISPLAY_DUMMY_H
#define DISPLAY_DUMMY_H

#include "controller.h" 

struct display_msg {
    float vel_kmh;
    tipo_veiculo_t tipo;
    status_t status;
};


void display_dummy_init(void);
void display_dummy_thread(void *a, void *b, void *c);
void display_dummy_show(float vel_kmh, tipo_veiculo_t tipo, status_t status);
void display_dummy_show_text(const char *text);

#endif 