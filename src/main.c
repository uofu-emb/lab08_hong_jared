#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define MASTER_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

static struct can2040 cbus;
QueueHandle_t msgs;

void master_task(void *args) {
    struct can2040_msg msg;
    printf("made it into master task\n");
    xQueueReceive(msgs, &msg, portMAX_DELAY);
    printf("Got message\n");
    // while(1) {
    //     printf("got the message\n");
    //     vTaskDelay(3000);
    // }

}

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    xQueueSendToBack(msgs, msg, portMAX_DELAY);
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
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

int main (void) 
{
    stdio_init_all();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t master;
    sleep_ms(10000);
    printf("made it after sleep\n");

    msgs = xQueueCreate(100, sizeof(struct can2040_msg));
    canbus_setup();
    //printf("can bus setup\n");

    //hard_assert(cyw43_arch_init() == PICO_OK);
    //printf("hard assert passed\n");

    xTaskCreate(master_task, "Master", configMINIMAL_STACK_SIZE, NULL,
        MASTER_TASK_PRIORITY, &master);
    vTaskStartScheduler();

    return 0;
}