#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include "rxtx.h"

// This struct is used by the can2040 libraries and must be included to communicate with
// the can 2040 API
static struct can2040 cbus;
QueueHandle_t msg_q;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg) {
    // If the message is a transmitted, nothing needs to be done, so we return.
    printf("got inside irq\n");
    if (notify == CAN2040_NOTIFY_TX) {
        printf("tx notify!\n");
        return;
    }
    // Else if it's a recieved message add it to the queue to be processed.
    else if (notify == CAN2040_NOTIFY_RX) {
        xQueueSendToBack(msg_q, msg, portMAX_DELAY);
    }
    else {
        printf("Can error\n");
    }
}

static void PIOx_IRQHandler(void) {
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void) {
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void master_task(void *args) {
    struct can2040_msg msg;
    TaskHandle_t rx, tx;
    int result;
    msg_q = xQueueCreate(100, sizeof(struct can2040_msg));
    canbus_setup();
    printf("canbus setup complete\n");

    // Create both transmit and receive tasks
    xTaskCreate(rx_task, "rx_task", configMINIMAL_STACK_SIZE, &msg_q,
        WORKER_TASK_PRIORITY, &rx);

    // This part of code will send messages. The reciever should have this for loop commented out.
    for (int i = 0; i < 30; i++) {
        // Get msg ready to sent ready
        msg.id = i;
        msg.dlc = 1;
        uint8_t input = i;
        msg.data[0] = i;

        printf("before check transmit\n");
        while (!can2040_check_transmit(&cbus)) {
            vTaskDelay(300);
        }

        printf("transmission line ready\n");
        result = can2040_transmit(&cbus, &msg);

        if (result == 0) {
            printf("message transmitted\n");
        }
        else {
            printf("message not transmitted\n");
        }
    }

    // Infinite loop
    while (1) {}
}

int main (void) {
    stdio_init_all();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t master;
    sleep_ms(10000);
    printf("made it after sleep\n");

    xTaskCreate(master_task, "Master", configMINIMAL_STACK_SIZE, NULL,
        MASTER_TASK_PRIORITY, &master);
    vTaskStartScheduler();

    return 0;
}