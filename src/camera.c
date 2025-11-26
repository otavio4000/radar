#include "camera.h"
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

LOG_MODULE_REGISTER(camera, LOG_LEVEL_INF);

void camera_init(void)
{
    LOG_INF("Camera inicializada (falha simulada = %d%%)",
            CONFIG_RADAR_CAMERA_FAILURE_RATE_PERCENT);
}

bool camera_process_frame(void)
{
    uint8_t sorte = sys_rand32_get() % 100;

    if (sorte < CONFIG_RADAR_CAMERA_FAILURE_RATE_PERCENT) {
        LOG_WRN("Falha simulada da câmera!");
        return false;
    }

    LOG_INF("Frame capturado com sucesso.");
    return true;
}

void camera_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    LOG_INF("Thread da câmera iniciada.");

    while (1) {
        camera_process_frame();
        k_sleep(K_SECONDS(2));
    }
}
