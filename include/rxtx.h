#define MASTER_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define WORKER_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

void rx_task(void *args);
void tx_task(void *args);