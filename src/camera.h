#ifndef CAMERA_H
#define CAMERA_H

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <stdbool.h>

void camera_init(void);
bool camera_process_frame(void);   
void camera_thread(void *, void *, void *);

#endif
