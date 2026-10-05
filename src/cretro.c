#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

#include "lcd.h"
#include "cpu.h"
#include "sound.h"
#include "timer.h"
#include "cli.h"
#include "logging.h"

int main(int argc, char **argv) {
	arg_conf *conf = cli_config_default();

	if (cli_config_handle(conf, argc, argv)) {
		cli_config_destroy(conf);
		return EXIT_FAILURE;
	}

	machine_init();

	rom_load(conf->rom_path);
	LOG_INFO("Successfully initialized ROM");

	lcd_init();
	LOG_INFO("Successfully initialized LCD");

	int video_pitch = sizeof(m.video[0]) * VIDEO_WIDTH;

	struct timeval cpu_clock_prev;
	gettimeofday(&cpu_clock_prev, NULL);

	// create timer thread to decrease delay- and sound timer
	pthread_t tid;
	pthread_create(&tid, NULL, &timer_update_callback, NULL);

	bool running = true;
	while (running) {
		running = lcd_process_input();

		handle_sound();

		struct timeval clock_now;
		gettimeofday(&clock_now, NULL);
		long dt = timediff_us(&clock_now, &cpu_clock_prev);
		if (dt > conf->us_delay) {
			cpu_step();
			lcd_step(m.video, video_pitch);
			cpu_clock_prev = clock_now;
		}
	}

	SDL_Quit();
	cli_config_destroy(conf);

	return EXIT_SUCCESS;
}
