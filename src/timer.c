#include "timer.h"

#define TIMER_DELAY (1000 / 60)

long timediff_us(const struct timeval *end, const struct timeval *start) {
	long diff = (end->tv_sec - start->tv_sec) * 1000000L + (end->tv_usec - start->tv_usec);
	return diff;
}

int msleep(long msec) {
	struct timespec ts;
	int res;

	if (msec < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec  = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}

static void update_timers(void) {
	// Decrease timers if necessary
	if (m.delay_timer > 0) {
		--m.delay_timer;
	}

	if (m.sound_timer > 0) {
		m.should_beep = 1;
		--m.sound_timer;
	} else {
		m.should_beep = 0;
	}
}

void *timer_update_callback(void *unused) {
	(void) unused;
	while (1) {
		msleep(TIMER_DELAY);
		update_timers();
	}
}
