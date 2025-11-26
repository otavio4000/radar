#ifndef IPC_H
#define IPC_H

#include <zephyr/kernel.h>
#include <stdint.h>

#define IPC_QUEUE_LEN 10

typedef struct {
    float vel_kmh;     // pode ser vazio dependendo do canal
    int tipo_veiculo;  // sensor/câmera pode ignorar
    int status;        // idem
} ipc_msg_t;

extern struct k_msgq sensor_to_controller_msgq;
extern struct k_msgq camera_to_controller_msgq;
extern struct k_msgq controller_to_display_msgq;

void ipc_init(void);

struct k_msgq *get_sensor_msgq(void);
struct k_msgq *get_camera_msgq(void);
struct k_msgq *get_display_msgq(void);

#endif
