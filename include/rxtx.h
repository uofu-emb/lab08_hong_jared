#define MASTER_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define WORKER_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

typedef struct task_args {
    struct can2040 *cd;   
} task_args;

void rx_task(int *args);
void tx_task(void *args);