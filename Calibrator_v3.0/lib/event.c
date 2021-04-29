#include "event.h"

void task_callback_register(struct task_cb *task, task_cb_t cb) {
    task->event_handler = cb;
}

void task_register(struct task_cb *task, unsigned char type, void *container, task_cb_t cb) {
    task->type = type;
    task->container = container;
    task->event_handler = cb;
    task->id = 0;
}

void task_set_type(struct task_cb *task, unsigned char type) {
    task->type = type;
}

void task_set_id(struct task_cb *task, unsigned char id) {
    task->id = id;
}