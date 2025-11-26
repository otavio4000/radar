#ifndef INPUT_THREAD_H
#define INPUT_THREAD_H

void input_thread_start(void);
void input_thread_stop(void);
void input_thread_set_callback(void (*cb)(void));

#endif 