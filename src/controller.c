#include "controller.h"
#include "sensors.h"
#include "ipc.h"                 
#include "vehicle_simulator.h"   
#include "display_dummy.h"       
#include <zephyr/logging/log.h>
#include <stdio.h>              


LOG_MODULE_REGISTER(controller, LOG_LEVEL_INF);


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
    float limite = (tipo == VEICULO_LEVE)
        ? CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH
        : CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;

    float tolerancia = limite * (CONFIG_RADAR_WARNING_THRESHOLD_PERCENT / 100.0f);
    float limite_amarelo = limite + tolerancia;

    if (vel_kmh > limite)
    {
        if (vel_kmh <= limite_amarelo)
            return STATUS_AMARELO;

        return STATUS_VERMELHO;
    }

    return STATUS_VERDE;
}


void controller_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);

    LOG_INF("Thread do controller iniciada. Aguardando dados/eventos...");

    struct sensor_msg sensor_msg;
    ipc_msg_t event_msg;
    ipc_msg_t camera_msg; 

    struct k_poll_event events[] = {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, &sensor_to_controller_msgq),
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, &event_to_controller_msgq),
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, &camera_to_controller_msgq)
    };

    while (1) {
        int ret = k_poll(events, ARRAY_SIZE(events), K_FOREVER);
        
        if (ret < 0) {
            LOG_ERR("Erro no k_poll: %d", ret);
            continue;
        }

        if (events[1].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
            if (k_msgq_get(&event_to_controller_msgq, &event_msg, K_NO_WAIT) == 0) { 

                if (event_msg.type == IPC_EVT_VEHICLE_REQUEST) {
                    
                    LOG_INF("EVENTO: Novo Veículo Acionado pelo Teclado. ID: %d", event_msg.data);

                    vehicle_simulator_press_enter(); 
                    
                    const char* new_plate = vehicle_simulator_generate();
                    
                    char display_text[64];
                    snprintf(display_text, sizeof(display_text), "SIMULATOR TRIGGERED. PLATE: %s", new_plate);
                    display_dummy_show_text(display_text); 
                }
            }
        }


        if (events[0].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
            if (k_msgq_get(&sensor_to_controller_msgq, &sensor_msg, K_NO_WAIT) == 0) {
                
                float vel = controller_calcula_velocidade_kmh(sensor_msg.tempo_ms);
                tipo_veiculo_t tipo = controller_classifica_eixos(sensor_msg.num_eixos);
                status_t status = controller_avalia_velocidade(vel, tipo);
                
                struct display_msg disp_msg = {
                    .vel_kmh = vel,
                    .tipo = tipo,
                    .status = status
                };
                
                if (k_msgq_put(&controller_to_display_msgq, &disp_msg, K_MSEC(50)) != 0) {
                    LOG_ERR("Falha ao enviar mensagem para o Display Dummy!");
                }
                
            
            }

        }

        
        if (events[2].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
            if (k_msgq_get(&camera_to_controller_msgq, &camera_msg, K_NO_WAIT) == 0) {
               
                // Aqui entraria a lógica de Reconhecimento de Placa ou processamento de imagem
                LOG_INF("CAMERA: Nova mensagem recebida. Iniciando processamento de placa...");
                
            }
        }

        for (size_t i = 0; i < ARRAY_SIZE(events); i++) {
            events[i].state = K_POLL_STATE_NOT_READY;
        }
    }
}