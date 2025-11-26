#ifndef IPC_H
#define IPC_H

#include <zephyr/kernel.h>
#include <stdint.h>

typedef enum {
    IPC_TYPE_SENSOR_DATA,      
    IPC_TYPE_CAMERA_DATA,      
    IPC_TYPE_DISPLAY_CMD,      
    IPC_EVT_VEHICLE_REQUEST,   
    IPC_EVT_SIMULATION_STOP    
} ipc_msg_type_t;

typedef struct {
    ipc_msg_type_t type;   
    uint32_t data;         
   
} ipc_msg_t;

extern struct k_msgq sensor_to_controller_msgq;
extern struct k_msgq camera_to_controller_msgq;
extern struct k_msgq controller_to_display_msgq;
extern struct k_msgq event_to_controller_msgq; 

void ipc_init(void);
struct k_msgq *get_sensor_msgq(void);
struct k_msgq *get_camera_msgq(void);
struct k_msgq *get_display_msgq(void);
int ipc_send_event(ipc_msg_t *msg);   
int ipc_receive_event(ipc_msg_t *msg, k_timeout_t timeout); 

#endif 