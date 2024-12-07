#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include "rxtx.h"

void rx_task(void *args) {
    while(1) {
        printf("running rx_task\n");
        vTaskDelay(3000);
    }
}

void tx_task(void *args) {
    while(1) {
        printf("running tx_task\n");
        vTaskDelay(3000);
    }
}