#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include "rxtx.h"

void rx_task(int *args) {
    while(1) {
        printf("running rx_task\n");
        //int *test = (int *)args; 
        printf("%d\n", *args);
        vTaskDelay(3000);
    }
}

void tx_task(void *args) {
    while(1) {
        printf("running tx_task\n");
        vTaskDelay(3000);
    }
}