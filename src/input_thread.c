#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>    
#include "input_thread.h"
#include "ipc.h" 

const struct device *const console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

extern int ipc_send_event(ipc_msg_t *msg); 


static void input_thread_fn(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    if (!device_is_ready(console_dev)) {
        printk("Input Thread ERROR: Console device not ready!\n");
        return;
    }

    printk("Input Thread: Aguardando 'Enter' para simular novo veiculo...\n");

    while (1)
    {
        unsigned char c;
        if (uart_poll_in(console_dev, &c) == 0) {
            
            if (c == '\n' || c == '\r') {
                
                printk("Input Thread: ENTER Pressionado! Acionando simulador...\n");

               
                ipc_msg_t msg = {
                    .type = IPC_EVT_VEHICLE_REQUEST, 
                    .data = k_uptime_get_32() 
                };

                int ret = ipc_send_event(&msg);
                
                if (ret != 0) {
                    printk("Input Thread ERROR: Falha ao enviar evento (%d). Fila cheia?\n", ret);
                }
            } else {
                uart_poll_out(console_dev, c); 
            }
        }
        
        k_msleep(10); 
    }
}


K_THREAD_STACK_DEFINE(input_stack, 1024);
static struct k_thread input_thread_data;

void input_thread_start(void)
{
    k_thread_create(
        &input_thread_data,
        input_stack, K_THREAD_STACK_SIZEOF(input_stack),
        input_thread_fn,
        NULL, NULL, NULL,
        7, 0, K_NO_WAIT
    );
    k_thread_name_set(&input_thread_data, "input_thread");
}

void input_thread_stop(void) {
    k_thread_abort(&input_thread_data);
}