#ifndef EVENT_H
#define EVENT_H

struct task_cb;

typedef void (*task_cb_t)(const struct task_cb *evt);

struct task_cb {
    task_cb_t event_handler;
    unsigned char type;
    unsigned char id;
    void *container;
};

typedef const struct task_cb task_t;

/**
 * Register callback function for processing events
 * @param task
 * @param cb callback function (must have task_t *evt as parameter)
 */
void task_callback_register(struct task_cb *task, task_cb_t cb);

/**
 * Register data associated with task
 * @param task
 * @param type task type
 * @param container frame container
 * @param cb callback function
 */
void task_register(struct task_cb *task, unsigned char type, void *container, task_cb_t cb);

/**
 * Set task type
 * @param task
 * @param type new task type
 */
void task_set_type(struct task_cb *task, unsigned char type);

/**
 * Set task Id
 * @param task
 * @param type new task type
 */
void task_set_id(struct task_cb *task, unsigned char id);

#endif /* EVENT_H */
