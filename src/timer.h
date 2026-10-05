#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include "cpu.h"
#include "sound.h"

int msleep(long msec);
_Noreturn void *timer_update_callback(void *unused);
__attribute__((pure)) long timediff_us(const struct timeval *end, const struct timeval *start);

#endif
