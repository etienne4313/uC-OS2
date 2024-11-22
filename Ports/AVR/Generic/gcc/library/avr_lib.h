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
#ifndef __AVR__LIB__H__
#define __AVR__LIB__H__

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

#define DIE(error_code) do {\
	die(error_code, __LINE__); \
} while(0)

#define DEBUG(fmt, args...) do {\
	if(debug) \
	fprintf(stderr, fmt, ## args); \
} while(0)

#define PRINT(fmt, args...) do {\
	fprintf(stderr, fmt, ## args); \
} while(0)

/******************************************************************************/
/* Library initialization */
/******************************************************************************/
extern void timer_init(void);
extern void uart_init(void);
static inline void lib_init(void)
{
	uart_init();
	PRINT("AVR init\n");
	timer_init();
}

/******************************************************************************/
/* Time */
/******************************************************************************/
#define USEC_PER_SEC 1000000UL
#define USEC_PER_MSEC 1000UL

#define DELAY_USEC(d) _delay_us(d)

typedef void(*work_t)(int arg, unsigned long usec_time);

struct work{
	work_t callback;
	int arg;
	unsigned long time;
	struct work *next;
};

unsigned long get_monotonic_cycle(void);
unsigned long get_monotonic_time(void);

/* Schedule work to happen at a specific absolute timestamp in usec */
void schedule_work_absolute(work_t s1, int arg, unsigned long timestamp);

/******************************************************************************/
/* Uart */
/******************************************************************************/
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
int USART_data_available(void);
int USART_Flush( void );

/******************************************************************************/
/* Watchdog */
/******************************************************************************/
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_4S     (_BV(WDP3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))
#define wdt_reset() __asm__ __volatile__ ("wdr")
static inline void watchdog_enable(uint8_t x)
{
	MCUSR = 0;
	WDTCSR = _BV(WDCE) | _BV(WDE);
	WDTCSR = x;
}

/******************************************************************************/
/* Math */
/******************************************************************************/
unsigned int divu10(unsigned int n);

#endif
