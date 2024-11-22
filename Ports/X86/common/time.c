#include <ucos_ii.h>

volatile u64 prev = 0, tick = 0;
u64 cycle_per_os_tick = 0, cycle_per_usec;

/* Calibrate timebase */
void time_calibration(void)
{
	int x;
	u64 t;

	PRINT("Calibrating\n");
	prev = get_monotonic_cycle();
	for(x = 0; x < 1000; x++)
		DELAY_USEC(1000);
	t = get_monotonic_cycle();
	cycle_per_usec = ((t - prev)/USEC_PER_SEC);
	cycle_per_os_tick = ((t - prev)/OS_TICKS_PER_SEC);
	PRINT("Calibration OS_TICKS_PER_SEC %d, CPU cycle %lld, CPU cyle per uSec %lld\n", OS_TICKS_PER_SEC, cycle_per_os_tick, cycle_per_usec);

	/* Establish base time stamp */
	prev = get_monotonic_cycle();
	tick = prev;
}


