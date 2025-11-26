#include "controller.h"
#include "sensors.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(controller, LOG_LEVEL_INF);

extern struct k_msgq sensor_msgq;

void controller_init(void)
{
    LOG_INF("Controller inicializado.");
}

float controller_calcula_velocidade_kmh(uint32_t tempo_ms)
{
    if (tempo_ms == 0) return 0.0f;

    float distancia_m = CONFIG_RADAR_SENSOR_DISTANCE_MM / 1000.0f;
    float tempo_s = tempo_ms / 1000.0f;

    float v_mps = distancia_m / tempo_s;
    float v_kmh = v_mps * 3.6f;
    return v_kmh;
}

tipo_veiculo_t controller_classifica_eixos(uint8_t num_eixos)
{
    if (num_eixos <= 2) return VEICULO_LEVE;
    return VEICULO_PESADO;
}

status_t controller_avalia_velocidade(float vel_kmh, tipo_veiculo_t tipo)
{
    int limite = (tipo == VEICULO_LEVE)
        ? CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH
        : CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;

    float amarelo_limite = limite * (CONFIG_RADAR_WARNING_THRESHOLD_PERCENT / 100.0f);

    if (vel_kmh <= amarelo_limite)
        return STATUS_VERDE;

    if (vel_kmh <= limite)
        return STATUS_AMARELO;

    return STATUS_VERMELHO;
}

void controller_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);

    LOG_INF("Thread do controller iniciada.");

    struct sensor_msg msg;

    while (1) {
        k_msgq_get(&sensor_msgq, &msg, K_FOREVER);

        float vel = controller_calcula_velocidade_kmh(msg.tempo_ms);
        tipo_veiculo_t tipo = controller_classifica_eixos(msg.num_eixos);
        status_t status = controller_avalia_velocidade(vel, tipo);

        LOG_INF("Eixos=%d Tempo=%d ms Vel=%.2f km/h -> %s",
                msg.num_eixos, msg.tempo_ms, vel,
                (status == STATUS_VERDE ? "VERDE" :
                 status == STATUS_AMARELO ? "AMARELO" : "VERMELHO"));
    }
}
