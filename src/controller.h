#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <zephyr/kernel.h>
#include <stdbool.h>
#include "ipc.h"
#include "sensors.h"


typedef enum {
    VEICULO_LEVE = 0,
    VEICULO_PESADO = 1
} tipo_veiculo_t;

typedef enum {
    STATUS_VERDE = 0,
    STATUS_AMARELO,
    STATUS_VERMELHO
} status_t;

void controller_init(void);
float controller_calcula_velocidade_kmh(uint32_t tempo_ms);
tipo_veiculo_t controller_classifica_eixos(uint8_t num_eixos);
status_t controller_avalia_velocidade(float vel_kmh, tipo_veiculo_t tipo);
void controller_thread(void *, void *, void *);

#endif
