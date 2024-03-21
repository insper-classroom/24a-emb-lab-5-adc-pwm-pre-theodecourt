#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int window[5] = {0}; // Janela para as últimas 5 amostras
    int sum = 0; // Soma das últimas 5 amostras
    int count = 0; // Contador para saber quantas amostras foram lidas
    int index = 0; // Índice para a próxima amostra

    while (true) {
        if (xQueueReceive(xQueueData, &data, portMAX_DELAY)) {
            // Se já tivermos 5 amostras, subtraímos a mais antiga da soma
            if (count >= 5) {
                sum -= window[index];
            }

            // Adicionamos a nova amostra na soma e na janela
            sum += data;
            window[index] = data;

            // Se já recebemos 5 ou mais amostras, calculamos a média
            if (count >= 5) {
                int average = sum / 5;
                printf("Média móvel: %d\n", average);
            } else {
                count++;
            }

            // Movemos o índice para a próxima posição
            index = (index + 1) % 5;

            // Deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
