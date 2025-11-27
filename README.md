
# Sistema de Radar Eletrônico – Zephyr RTOS

Este projeto implementa um Radar Eletrônico para Controle de Velocidade utilizando o Zephyr Real-Time Operating System (RTOS).
O objetivo é simular, em tempo real:

- Detecção de veículos
- Cálculo de velocidade
- Contagem de eixos
- Classificação de veículos
- Acionamento da câmera
- Registro de infrações
- Exibição no display
- Execução assíncrona, determinística e com prioridades RTOS.

## Tecnologias & Conceitos Utilizados

### 1. Agendamento e Prioridades de Thread (RTOS)

O Zephyr utiliza um escalonador preemptivo baseado em prioridades.

Prioridades usadas:
- Controller (5 – Alta): Cálculo, classificação e decisão
- Display (6): Exibição
- Sensors (7): Detecção
- Camera (7): OCR da placa

### 2. Comunicação entre Threads – k_msgq

O sistema usa k_msgq para troca assíncrona de dados entre threads.

Benefícios:
- Elimina polling
- Evita memória compartilhada insegura
- Payloads fixos com tempo determinístico

Payloads utilizados:
- raw_detection_data_t
- plate_data_t
- camera_trigger_t
- display_msg_t

### 3. Sincronização com k_poll

A controller_thread utiliza:
k_poll(events, 3, K_FOREVER);

Isso permite:
- Dormir enquanto espera eventos
- Acordar na chegada de novos dados
- Baixo consumo de CPU
- Comportamento RTOS real

## Arquitetura do Sistema

### Fluxo de Dados

Sensors Thread → raw_detection_data_t  
Controller Thread → processa, decide e envia display_msg_t ou camera_trigger_t  
Camera Thread → plate_data_t  
Display Thread → exibição final

### Etapas do Processamento

Detecção:
- Envio de raw_detection_data_t { tempo_ms, num_eixos }

Decisão:
- Cálculo de velocidade
- Classificação por eixos
- Avaliação da infração
- Status: VERDE, AMARELO, VERMELHO

Ações:
- Verde/Amarelo → display
- Vermelho → acionamento da câmera e espera da placa

Finalização:
- Camera retorna plate_data_t
- Controller envia display_msg_t ao display

## Execução do Projeto (QEMU)

Placa: mps2_an385 (ARM Cortex-M3)

### Comandos

1. Limpar build:
rm -rf build

2. Gerar build:
west build -b mps2_an385 /home/taisa/emb-base-sample

3. Executar no QEMU:
west build -t run

# Visão Geral da Estrutura de Código

A organização em arquivos `.c` (implementação) e `.h` (cabeçalhos/definições) facilita a gestão de dependências e a reutilização de código, seguindo o princípio da modularidade.

---

## Arquivos Principais

| Arquivo | Tipo | Função no Projeto | Módulos Relacionados |
|--------|------|-------------------|-----------------------|
| **main.c** | Principal | Ponto de entrada. Inicializa o Zephyr, o sistema IPC e dispara todas as threads (Controller, Sensors, Camera, Display, Input). | Todos os `.h` |
| **controller.c** | Thread RTOS | **Cérebro do Radar.** Implementa a `controller_thread`, a lógica de cálculo de velocidade, classificação, avaliação de infração e decisão de acionamento da câmera. | `ipc.h`, `controller.h` |
| **sensors.c** | Thread RTOS | Simulação da detecção física. Implementa a `sensors_thread`, que simula a passagem dos eixos e envia os dados brutos (`raw_detection_data_t`). | `ipc.h`, `sensors.h` |
| **camera.c** | Thread RTOS | Simulação do processamento de imagem/OCR. Implementa a `camera_thread`, que espera um gatilho e simula a leitura da placa (`plate_data_t`). | `ipc.h`, `camera.h`, `PlateGenerator.c` |
| **display_dummy.c** | Thread RTOS | Simulação da interface de usuário. Implementa a `display_thread`, que espera mensagens finais e exibe o status da infração/veículo. | `ipc.h`, `display_dummy.h` |
| **ipc.c** | Módulo RTOS | Infraestrutura de comunicação. Define e inicializa objetos do Zephyr para comunicação, como **k_msgq** (filas de mensagens) e eventos **k_poll**. | `ipc.h` |

---

# Detalhamento dos Módulos-Chave (A Lógica do RTOS)

## 1. Módulos de Comunicação (IPC - *Inter-Process Communication*)
**Arquivos:** `ipc.h` / `ipc.c`

Este módulo é o coração do sistema reativo.  
Ele define:

- Estruturas de dados (`raw_detection_data_t`, `plate_data_t`, `camera_trigger_t`, etc.)
- Filas de mensagens (`k_msgq`)
- Eventos usados pelo `k_poll`

Exemplo provável em `ipc.c`:

```c
K_MSGQ_DEFINE(sensor_msgq, sizeof(raw_detection_data_t), 10, 4);

```

## 2. Módulo de Controle (controller.c)
Este arquivo contém a função mais complexa do sistema: o loop principal da controller_thread.

Sincronização (k_poll): O código utiliza a função k_poll para aguardar eventos de múltiplas fontes (Sensores e Câmera) sem gastar CPU.

```c

// Dentro de controller.c, na thread loop:
k_poll(events, 3, K_FOREVER); // Dorme até que dados cheguem em uma das 3 fontes.
Lógica de Decisão:
```

Se o evento vier do Sensor: Executa as funções de cálculo.

Se a velocidade for alta (Status VERMELHO): Envia uma mensagem de gatilho (camera_trigger_t) para a fila da Câmera.

Se o evento vier da Câmera (retorno da placa): Finaliza o registro e envia a mensagem final (display_msg_t) para o Display.

## 3. Módulos de Simulação (sensors.c, camera.c)
Esses arquivos simulam a interação com o mundo real, desacoplando o código do hardware físico.

sensors.c: Simula a chegada de dados. O código gera números pseudo-aleatórios ou sequenciais para o tempo_ms e num_eixos, e então empacota esses dados e envia via k_msgq_put.

camera.c: Após receber um gatilho, ele chama a função do PlateGenerator e simula o tempo de OCR com um k_sleep(), antes de enviar a placa lida de volta ao Controller.

## 4. Módulo de Entrada Auxiliar (input_thread.c)

Dispara um evento de teste manualmente.


## Alunos

- Taisa Lima 
- Otavio Barbosa


