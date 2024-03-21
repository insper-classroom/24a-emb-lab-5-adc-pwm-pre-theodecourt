#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
#define WINDOW_SIZE 5
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
    int window[WINDOW_SIZE] = {0}; // Janela para armazenar as últimas 5 amostras
    int sum = 0; // Soma das últimas 5 amostras
    int count = 0; // Contador para saber quantas amostras foram lidas
    int index = 0; // Índice para inserir a próxima amostra

    while (true) {
    // Se já tivermos 5 amostras, subtraímos a mais antiga da soma
        if (count >= WINDOW_SIZE) {
            sum -= window[index];
        } else {
            // Se ainda não temos 5 amostras, apenas incrementamos o contador
            count++;
        }

        // Adicionamos a nova amostra na soma e no vetor
        sum += data;
        window[index] = data;

        // Calculamos a média móvel e imprimimos o resultado
        if (count >= WINDOW_SIZE) {
            printf("Média móvel: %d\n", sum / WINDOW_SIZE);
        }

        // Atualizamos o índice para a próxima inserção
        index = (index + 1) % WINDOW_SIZE;

        // deixar esse delay!
        vTaskDelay(pdMS_TO_TICKS(50));
        
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
