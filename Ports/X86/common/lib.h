/*
 * Copyright 2024, Etienne Martineau etienne4313@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __LIB__H__
#define __LIB__H__

/******************************************************************************/
/* Debug & Error handling */
/* All the \c printf and \c scanf family functions come in two flavours: the
 * standard name, where the format string is expected to be in
 * SRAM, as well as a version with the suffix "_P" where the format
 * string is expected to reside in the flash ROM.  The macro
 * \c PSTR (explained in \ref avr_pgmspace) becomes very handy
 * for declaring these format strings.
 */
/******************************************************************************/
extern int debug;
extern void die(int err, int line);

enum thin_lib_error_condition{
	TIMER = -15,
	IRQ = -14,
	FATAL = -13,
};

#ifdef __KERNEL__
#define PRINTF printk
#else
#define PRINTF printf
#endif

#define DIE(error_code) do {\
	die(error_code, __LINE__); \
} while(0)

#define DEBUG(format, ...) do {\
	if(debug) \
		PRINTF(format, ## __VA_ARGS__);\
} while(0)

#define PRINT(format, ...) do {\
	PRINTF(format, ## __VA_ARGS__);\
} while(0)

/******************************************************************************/
/* Library initialization */
/******************************************************************************/
extern void time_calibration(void);
extern volatile u64 prev, tick;
extern u64 cycle_per_os_tick, cycle_per_usec;
static inline void lib_init(void)
{
	time_calibration();
	PRINT("lib init\n");
}

/******************************************************************************/
/* Time */
/******************************************************************************/
#ifdef __KERNEL__
#define DELAY_USEC(d) udelay(d)
#else
#define USEC_PER_SEC 1000000UL
#define USEC_PER_MSEC 1000UL
#define DELAY_USEC(d) usleep(d)
#endif

typedef void(*work_t)(int arg, unsigned long usec_time);

struct work{
	work_t callback;
	int arg;
	unsigned long time;
	struct work *next;
};

#ifdef __KERNEL__
static inline u64 get_monotonic_cycle(void)
{
	return rdtsc();
}
#else
#define DECLARE_ARGS(val, low, high)    unsigned low, high
#define EAX_EDX_VAL(val, low, high) ((low) | ((u64)(high) << 32))
#define EAX_EDX_ARGS(val, low, high)    "a" (low), "d" (high)
#define EAX_EDX_RET(val, low, high) "=a" (low), "=d" (high)
static inline u64 get_monotonic_cycle(void)
{
    DECLARE_ARGS(val, low, high);
    asm volatile("rdtsc" : EAX_EDX_RET(val, low, high));
    return EAX_EDX_VAL(val, low, high);
}
#endif

static inline unsigned long get_monotonic_time(void)
{
	u64 t = get_monotonic_cycle();
	t = t / cycle_per_usec;
	return t;
}

/* Schedule work to happen at a specific absolute timestamp in usec */
static inline void schedule_work_absolute(work_t s1, int arg, unsigned long timestamp)
{
}

/******************************************************************************/
/* Uart */
/******************************************************************************/

/******************************************************************************/
/* Watchdog */
/******************************************************************************/

/******************************************************************************/
/* Math */
/******************************************************************************/

#endif
