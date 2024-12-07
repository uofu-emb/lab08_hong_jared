#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include "rxtx.h"

void rx_task(QueueHandle_t *msg_q) {
    struct can2040_msg msg;
    char c;
    while(1) {
        printf("Worker task waiting for message\n");
        xQueueReceive(*msg_q, &msg, portMAX_DELAY);
        c = msg.data[0];
        printf("message is: %c\n", c);
    }
}
