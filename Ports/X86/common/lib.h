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
extern void timer_init(void);
extern volatile u64 prev, tick;
extern u64 cycle_per_os_tick, cycle_per_usec;
static inline void lib_init(void)
{
	PRINT("lib init\n");
	timer_init();
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

struct w{
	work_t callback;
	int arg;
	unsigned long time;
	struct w *next;
};

u64 get_monotonic_cycle(void);
unsigned long get_monotonic_time(void);
void poll_timer(u64 t);

/* Schedule work to happen at a specific absolute timestamp in usec */
void schedule_work_absolute(work_t s1, int arg, unsigned long timestamp);

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
