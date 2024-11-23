#include <ucos_ii.h>

volatile u64 prev = 0, tick = 0;
u64 cycle_per_os_tick = 0, cycle_per_usec;

/*
 *********************************************************************************************************
 * Monotonic Time
 *********************************************************************************************************
 */
#define MONOTONIC_CYCLE_TO_USEC(x) ( x / cycle_per_usec )
#define USEC_TO_MONOTONIC_CYCLE(x) ( x * cycle_per_usec )

#ifdef __KERNEL__
u64 get_monotonic_cycle(void)
{
	return rdtsc();
}
#else
#define DECLARE_ARGS(val, low, high)    unsigned low, high
#define EAX_EDX_VAL(val, low, high) ((low) | ((u64)(high) << 32))
#define EAX_EDX_ARGS(val, low, high)    "a" (low), "d" (high)
#define EAX_EDX_RET(val, low, high) "=a" (low), "=d" (high)
u64 get_monotonic_cycle(void)
{
    DECLARE_ARGS(val, low, high);
    asm volatile("rdtsc" : EAX_EDX_RET(val, low, high));
    return EAX_EDX_VAL(val, low, high);
}
#endif

unsigned long get_monotonic_time(void)
{
	u64 t = get_monotonic_cycle();
	t = t / cycle_per_usec;
	return t;
}

/*
 *********************************************************************************************************
 * Timer wheel
 *********************************************************************************************************
 */

/* Static table */
#define MAX_SCHEDULE 12
static struct w work_table[MAX_SCHEDULE];
static struct w *current_w;

static inline void timer_wheel_disable(void)
{
}
static inline void timer_wheel_enable(void)
{
}

static inline void timer_wheel_set(unsigned short t)
{
}

static void prog_timer(unsigned long cycle)
{
}

/*
 * NOTE this IRQ is handled by gcc hence we are not expecting any RTOS context switch to happen.
 * This IRQ drives the timer wheel and process the callback
 */
void poll_timer(u64 t)
{
	if(!current_w)
		return;
	if(current_w->time > t){ /* Some time left */
		return;
	}
expiry:
	/* Expiry process the callback */
	if(!current_w->callback)
		DIE(TIMER);
	current_w->callback(current_w->arg, MONOTONIC_CYCLE_TO_USEC(t));
	current_w->callback = NULL; /* Mark this callback completed */

	if(!current_w->next){ /* The end */
		timer_wheel_disable(); /* Stop the timer */
		current_w = NULL;
		return;
	}

	current_w = current_w->next; /* Get to the next schedule */
	if(current_w->time > t){ /* Some time left */
		prog_timer(current_w->time - t);
		return;
	}
	goto expiry;
}

void schedule_work_absolute(work_t s1, int arg, unsigned long timestamp)
{
	int x;
	unsigned long t;
	struct w *a, *prev, *curr;

	for(x=0; x<MAX_SCHEDULE; x++){ /* Find an empty spot */
		a = &work_table[x];
		if(a->callback) /* Schedule is still active */
			continue;
		break;
	}
	if(a->callback)
		DIE(TIMER);

	/* Put everything in Timer Cycle */
	timestamp = USEC_TO_MONOTONIC_CYCLE(timestamp);

	a->callback = s1;
	a->arg = arg;
	a->time = timestamp;
	a->next = NULL;

	t = get_monotonic_cycle(); /* Get current timestamp */
	if(timestamp <= t){ /* Past expiration ?  process the callback */
		if(!a->callback)
			DIE(TIMER);
		a->callback(a->arg, MONOTONIC_CYCLE_TO_USEC(t));
		a->callback = NULL; /* Mark this callback completed */
		return;
	}

	if(!current_w){ /* Nothing is running so start fresh */
		timer_wheel_disable(); /* Make sure the timer is stopped */
		current_w = a; /* Set current schedule to be this one */
		prog_timer(current_w->time - t); /* Program the timer */
		timer_wheel_enable(); /* Start the timer */
		return;
	}

	/* 
	 * There is an active schedule but this schedule will expire sooner than
	 * the current one so reprogram the timer with this new one
	 */
	if(timestamp < current_w->time){
		prev = current_w; /* Remember current_w */
		current_w = a; /* Replace current_w with this one */
		current_w->next = prev; /* Old current_w is the next one */
		prog_timer(current_w->time - t); /* Program the timer */
		return;
	}

	/* 
	 * This schedule will expire later than the current one so insert time sorted
	 * in the list. We don't need to reprogram the timer here
	 */
	curr = current_w;
	while(curr != NULL){ /* Traverse the list */
		if(!curr->next)
			break; /* Insert at the end */
		prev = curr;
		curr = curr->next;
		if(timestamp < curr->time){
			a->next = curr;
			curr = prev;
			break; /* Insert in the middle */
		}
	}
	curr->next = a;
	return;
}

void timer_init(void)
{
	int x;
	u64 t;

	PRINT("Calibrating\n");
	prev = get_monotonic_cycle();
	for(x = 0; x < 1000; x++)
		DELAY_USEC(1000);
	t = get_monotonic_cycle();
	cycle_per_os_tick = ((t - prev)/OS_TICKS_PER_SEC);
	PRINT("Calibration OS_TICKS_PER_SEC %d, CPU cycle %lld\n", OS_TICKS_PER_SEC, cycle_per_os_tick);

	cycle_per_usec = 0;
	prev = get_monotonic_cycle();
	for(x = 0; x < 1024; x++)
		DELAY_USEC(1000);
	t = get_monotonic_cycle();
	cycle_per_usec += (t - prev);
	cycle_per_usec = (cycle_per_usec / 1000) / 1024;
	PRINT("Calibration CPU cyle per uSec %lld\n", cycle_per_usec);

	/* Establish base time stamp */
	prev = get_monotonic_cycle();
	tick = prev;

	timer_wheel_disable();
	memset(work_table, 0, sizeof(struct w) * MAX_SCHEDULE);
	current_w = NULL;
}

