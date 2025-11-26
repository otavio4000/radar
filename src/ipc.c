#include "ipc.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define IPC_QUEUE_LEN 8 

static ipc_msg_t event_to_controller_buffer[IPC_QUEUE_LEN];
struct k_msgq event_to_controller_msgq; 
static ipc_msg_t sensor_to_controller_buffer[IPC_QUEUE_LEN];
static ipc_msg_t camera_to_controller_buffer[IPC_QUEUE_LEN];
static ipc_msg_t controller_to_display_buffer[IPC_QUEUE_LEN];

struct k_msgq sensor_to_controller_msgq;
struct k_msgq camera_to_controller_msgq;
struct k_msgq controller_to_display_msgq;


void ipc_init(void)
{
    printk("[IPC] Inicializando canais IPC...\n");

    k_msgq_init(&event_to_controller_msgq,
                (char *)event_to_controller_buffer,
                sizeof(ipc_msg_t),
                IPC_QUEUE_LEN);

    k_msgq_init(&sensor_to_controller_msgq,
                (char *)sensor_to_controller_buffer,
                sizeof(ipc_msg_t),
                IPC_QUEUE_LEN);

    k_msgq_init(&camera_to_controller_msgq,
                (char *)camera_to_controller_buffer,
                sizeof(ipc_msg_t),
                IPC_QUEUE_LEN);

    k_msgq_init(&controller_to_display_msgq,
                (char *)controller_to_display_buffer,
                sizeof(ipc_msg_t),
                IPC_QUEUE_LEN);

    printk("[IPC] Message queues prontas.\n");
}



int ipc_send_event(ipc_msg_t *msg)
{
    return k_msgq_put(&event_to_controller_msgq, msg, K_NO_WAIT);
}

int ipc_receive_event(ipc_msg_t *msg, k_timeout_t timeout)
{
    return k_msgq_get(&event_to_controller_msgq, msg, timeout);
}


struct k_msgq *get_sensor_msgq(void)
{
    return &sensor_to_controller_msgq;
}
struct k_msgq *get_camera_msgq(void)
{
    return &camera_to_controller_msgq;
}
struct k_msgq *get_display_msgq(void)
{
    return &controller_to_display_msgq;
}