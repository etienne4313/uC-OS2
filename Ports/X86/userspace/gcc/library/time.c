#include <ucos_ii.h>

/* Calibrate timebase */
void time_calibration(void)
{
	int x;
	u64 t;

	PRINT("Calibrating\n");
	prev = get_monotonic_cycle();
	for(x = 0; x < 1000; x++)
		usleep(1000);
	t = get_monotonic_cycle();
	cycle_per_os_tick = ((t - prev)/OS_TICKS_PER_SEC);
	PRINT("Calibration OS_TICKS_PER_SEC %d, CPU cycle %lld\n", OS_TICKS_PER_SEC, cycle_per_os_tick);

	/* Establish base time stamp */
	prev = get_monotonic_cycle();
	tick = prev;
}


