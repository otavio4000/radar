#include "vehicle_simulator.h"
#include "../../src/ipc.h"
#include "../../src/sensors.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <autoconf.h>     
#include <stdlib.h>     
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>     

LOG_MODULE_REGISTER(vehicle_sim, LOG_LEVEL_INF);

#define SIM_THREAD_PRIO 5
#define SIM_THREAD_STACK_SIZE 2048

static K_THREAD_STACK_DEFINE(sim_stack, SIM_THREAD_STACK_SIZE);
static struct k_thread sim_thread_data;
static struct k_sem sim_stop_sem;

/* controle */
static volatile int sim_running = 0;


static K_SEM_DEFINE(sim_enter_sem, 0, 1);

static uint32_t rand_range(uint32_t a, uint32_t b)
{
    if (b <= a) return a;
    return a + (rand() % (b - a + 1));
}

static uint32_t calc_tempo_ms(uint32_t dist_mm, float speed_kmh)
{
    if (speed_kmh <= 0) return 1;
    float t = (dist_mm * 3.6f) / speed_kmh;
    return (uint32_t)(t + 0.5f);
}

static char plate_buffer[8]; 


const char* vehicle_simulator_generate(void)
{
    
    srand(k_uptime_get_32() ^ 0xDEADBEEF); 
    
    plate_buffer[0] = 'A' + (rand() % 26);
    plate_buffer[1] = 'A' + (rand() % 10);
    plate_buffer[2] = 'A' + (rand() % 10);
    plate_buffer[3] = '0' + (rand() % 10);
    plate_buffer[4] = 'A' + (rand() % 26);
    plate_buffer[5] = '0' + (rand() % 10);
    plate_buffer[6] = '0' + (rand() % 10);
    plate_buffer[7] = '\0';
    
    return plate_buffer;
}

static void sim_thread_fn(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    srand(k_uptime_get_32());

    while (sim_running)
    {
        k_sem_take(&sim_enter_sem, K_FOREVER);

        if (!sim_running)
            break;
        
        LOG_DBG("SIMULATOR THREAD: Acionado pelo semáforo. Gerando dados do sensor...");

        bool is_heavy = (rand() % 100) < 30;  /* 30% chance de ser pesado */
        uint8_t num_eixos = is_heavy ? rand_range(3, 6) : 2;

       
        uint32_t limit_kmh = is_heavy ?
            CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH :
            CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH;

        float speed_kmh;

        // 30% de chance de infração
        if ((rand() % 100) < 30) 
        {
            speed_kmh = limit_kmh + rand_range(5, 30);
            LOG_WRN("SIMULATOR: Gerada infração de velocidade! (%f km/h)", speed_kmh);
        }
        else
        {
            speed_kmh = rand_range(limit_kmh * 0.60f, limit_kmh * 0.95f);
        }

        uint32_t tempo_ms = calc_tempo_ms(CONFIG_RADAR_SENSOR_DISTANCE_MM, speed_kmh);

        struct sensor_msg msg = {
            .num_eixos = num_eixos,
            .tempo_ms = tempo_ms
        };

        struct k_msgq *q = get_sensor_msgq();

        if (q)
        {
            k_msgq_put(q, &msg, K_MSEC(50));  
            LOG_DBG("SIM: Pulso de sensor (Eixos=%d, Tempo=%dms) enviado.", num_eixos, tempo_ms);
        }

        k_yield();
    }

    k_sem_give(&sim_stop_sem);
}


void vehicle_simulator_press_enter(void)
{

    k_sem_give(&sim_enter_sem);
}

void vehicle_simulator_start(void)
{
    if (sim_running) return;

    sim_running = 1;
    k_sem_init(&sim_stop_sem, 0, 1);

    k_thread_create(
        &sim_thread_data,
        sim_stack, K_THREAD_STACK_SIZEOF(sim_stack),
        sim_thread_fn,
        NULL, NULL, NULL,
        SIM_THREAD_PRIO, 0, K_NO_WAIT
    );

    k_thread_name_set(&sim_thread_data, "vehicle_sim");
    LOG_INF("Simulador de veículos (Thread) iniciado.");
}

void vehicle_simulator_stop(void)
{
    if (!sim_running) return;

    sim_running = 0;

    k_sem_give(&sim_enter_sem);

    k_sem_take(&sim_stop_sem, K_SECONDS(2));
    LOG_INF("Simulador de veículos parado.");
}