#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ipc.h"
#include "sensors.h"
#include "controller.h"
#include "camera.h"     
#include "display_dummy.h" 
#include "vehicle_simulator.h" 
#include "input_thread.h"    

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);


K_THREAD_DEFINE(sensors_thread_def, 1024, sensors_thread, NULL, NULL, NULL, 
                7, 0, 0);

K_THREAD_DEFINE(controller_thread_def, 2048, controller_thread, NULL, NULL, NULL,
                5, 0, 0);
                
K_THREAD_DEFINE(display_thread_def, 1024, display_dummy_thread, NULL, NULL, NULL,
                6, 0, 0);


int main(void)
{
    LOG_INF("========================================");
    LOG_INF(" Radar Eletrônico - Sistema Iniciado");
    LOG_INF(" Plataforma: mps2_an385 (QEMU)");
    LOG_INF("========================================");

    LOG_INF("Inicializando IPC...");
    ipc_init();  
    LOG_INF("IPC OK.");

    LOG_INF("Inicializando módulos...");

    sensors_init();
    controller_init();
    camera_init();
    display_dummy_init();

    LOG_INF("Todos os módulos foram inicializados.");

    vehicle_simulator_start(); 
    input_thread_start();

    LOG_INF("Sistema operacional ativo. Threads rodando.");
    
   
    while (1) {
        
        k_sleep(K_FOREVER);
    }

    return 0;
}