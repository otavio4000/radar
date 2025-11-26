#include "display_dummy.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include "ipc.h"   

LOG_MODULE_REGISTER(display_dummy, LOG_LEVEL_INF);

struct display_msg {
    float vel_kmh;
    tipo_veiculo_t tipo;
    status_t status;
};

// ANSI colors
#define ANSI_RESET   "\033[0m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_RED     "\033[31m"

void display_dummy_init(void)
{
    LOG_INF("Display Dummy inicializado (console).");
}

void display_dummy_show(float vel_kmh, tipo_veiculo_t tipo, status_t status)
{
    const char *cor;
    const char *status_txt;

    switch (status) {
    case STATUS_VERDE:
        cor = ANSI_GREEN;
        status_txt = "OK";
        break;
    case STATUS_AMARELO:
        cor = ANSI_YELLOW;
        status_txt = "ATENÇÃO";
        break;
    default:
        cor = ANSI_RED;
        status_txt = "INFRAÇÃO";
        break;
    }

    const char *tipo_txt = (tipo == VEICULO_LEVE) ? "LEVE" : "PESADO";

    printk("\n%s====================== RADAR ======================%s\n",
           cor, ANSI_RESET);

    printk("%sVelocidade: %.2f km/h%s\n", cor, vel_kmh, ANSI_RESET);
    printk("%sTipo: %s%s\n", cor, tipo_txt, ANSI_RESET);
    printk("%sStatus: %s%s\n", cor, status_txt, ANSI_RESET);

    printk("%s==================================================%s\n",
           cor, ANSI_RESET);
}

void display_dummy_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    LOG_INF("Thread do display iniciada.");

    struct display_msg msg;

    while (1) {
        k_msgq_get(&controller_to_display_msgq, &msg, K_FOREVER);
        display_dummy_show(msg.vel_kmh, msg.tipo, msg.status);
    }
}
