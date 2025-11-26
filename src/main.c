#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ipc.h"
#include "sensors.h"
#include "controller.h"
#include "camera.h"
#include "display_dummy.h"

LOG_MODULE_REGISTER(main_mod, LOG_LEVEL_INF);

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
    LOG_INF("Sistema operacional ativo. Threads rodando.");

    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
