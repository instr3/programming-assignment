#include "common.h"
void init_monitor(int, char *[]);
void init_caches();
void reg_test();
void restart();
void init_device();
void init_sdl();
void ui_mainloop();

int main(int argc, char *argv[]) {



	/* Initialize the monitor. */
	init_monitor(argc, argv);

#ifdef USE_CACHE
	/* Init 2 caches.*/
	init_caches();
#endif

	/* Test the implementation of the ``CPU_state'' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart();

	/* Initialize devices. */
	init_device();
	init_sdl();

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
