#Sistema de Radar Eletrônico – Zephyr RTOS

Este projeto implementa um Radar Eletrônico para Controle de Velocidade utilizando o Zephyr Real-Time Operating System (RTOS).
O objetivo é simular, em tempo real:

- Detecção de veículos

- Cálculo de velocidade

- Contagem de eixos

- Classificação de veículos

- Acionamento da câmera

- Registro de infrações

- Exibição no display

- Tudo isso de forma assíncrona, determinística e com prioridades de execução real-time.

#Tecnologias & Conceitos Utilizados

##1. Agendamento e Prioridades de Thread (RTOS)

O Zephyr utiliza um escalonador preemptivo baseado em prioridades.

Prioridades usadas:
Thread	Prioridade	Função
Controller	5 (alta)	Cálculo, classificação e decisão
Display	6	Exibição
Sensors	7	Detecção
Camera	7	OCR da placa

➡️ O Controller possui prioridade mais alta, garantindo menor latência e resposta imediata aos eventos.

##2. Comunicação entre Threads: k_msgq (Filas de Mensagens)

O sistema usa k_msgq para troca assíncrona de dados entre threads.

Benefícios:

Elimina polling

Evita memória compartilhada insegura

Usa payloads fixos, garantindo tempo determinístico de cópia

As filas são definidas em ipc.c e declaradas como extern em ipc.h

Payloads usados:

raw_detection_data_t

plate_data_t

camera_trigger_t

display_msg_t

###3.  Sincronização com k_poll

A controller_thread age como o “hub” do sistema.

Ela utiliza:

k_poll(events, 3, K_FOREVER);


Isso permite:

Dormir enquanto espera eventos

Acordar imediatamente quando sensores ou câmera mandam dados

Não gastar CPU inutilmente

Garantindo comportamento realmente RTOS.

#Arquitetura do Sistema

O sistema funciona como um pipeline reativo baseado em mensagens.

1. Fluxo de Dados
Componente	Entrada	Saída	Função
Sensors Thread	—	raw_detection_data_t	Simula eixos e tempo
Controller Thread	dados de sensores ou câmera	display_msg_t ou camera_trigger_t	Processa e decide
Camera Thread	camera_trigger_t	plate_data_t	Simula OCR
Display Thread	display_msg_t	—	Exibição final
2. Etapas do Processamento
   ###Detecção (Sensors Thread)

Envia:

raw_detection_data_t { tempo_ms, num_eixos }

###Decisão (Controller Thread)

Executa:

Cálculo de velocidade

Classificação por eixos

Avaliação da infração

Define:

STATUS_VERDE
STATUS_AMARELO
STATUS_VERMELHO

###Ações
Status	Ação
Verde / Amarelo	Envia direto para display
Vermelho	Aciona câmera via camera_trigger_t e espera a placa

###Finalização

Câmera retorna:

plate_data_t { placa, país }


Controller monta:

display_msg_t


E envia ao display.

🚀 Execução do Projeto (QEMU)

O projeto roda em:

mps2_an385 (ARM Cortex-M3)

📦 Pré-requisitos

Zephyr SDK + Toolchain

west (meta-tool do Zephyr)

🛠️ Comandos
1. Limpar build (recomendado)
rm -rf build

2. Gerar build
west build -b mps2_an385 /home/taisa/emb-base-sample

3. Executar no QEMU
west build -t run


O console exibirá:

- Logs das threads

- Fluxo de mensagens

- Simulação de veículos

- Detecções, decisões e placas
