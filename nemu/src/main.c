void init_monitor(int, char *[]);
void init_caches();
void reg_test();
void restart();
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

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
