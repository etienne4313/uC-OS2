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
#include <ucos_ii.h>

OS_STK *original_stack; /* Hold the original stack just before starting the first thread */

void OSTaskCreateHook(OS_TCB *ptcb){}
void OSTaskDelHook (OS_TCB *ptcb){}
void OSTaskSwHook (void){}
void OSTaskStatHook (void){}
void OSTaskReturnHook(OS_TCB *ptcb){}
void OSSystemReset(void){}
void OSTCBInitHook(OS_TCB *ptcb){}
void OSTimeTickHook (void){}
void OSInitHookBegin(void){}
void OSInitHookEnd(void){}

OS_STK* OSTaskStkInit (void (*task)(void* pd), void* pdata, OS_STK* ptos, INT16U opt)
{
	*ptos-- = (OS_STK)task; // RIP
	*ptos-- = 0;    // R8
	*ptos-- = 0;    // R9
	*ptos-- = 0;    // R10
	*ptos-- = 0;    // R11
	*ptos-- = 0;    // R12
	*ptos-- = 0;    // R13
	*ptos-- = 0;    // R14
	*ptos-- = 0;    // R15
	*ptos-- = 0;    // RAX
	*ptos-- = 0;    // RBX
	*ptos-- = 0;    // RCX
	*ptos-- = 0;    // RDX
	*ptos-- = 0;    // RSI
	*ptos-- = (unsigned long)pdata;   // RDI
	*ptos = 0;  	// RBP
//	DEBUG("PTOS %lx\n", (unsigned long)ptos);
	return ptos;
}

/* OSStart -> OSStartHighRdy  */
void OSStartHighRdy(void)
{
	OSTaskSwHook();
	OSRunning = OS_TRUE;

	/* 
	 * OSTCBCur == OSTCBHighRdy; OSPrioCur == OSPrioHighRdy;
	 *
	 * This function will return back to the caller 'OSStart' _only_
	 * if some thread calls into EXIT()
	 */
	__start_to_asm(OSTCBHighRdy->OSTCBStkPtr, &original_stack);
}

/* OSIntEnter / OSIntExit -> OSIntCtxSw */
void OSIntCtxSw(void)
{
	OS_TCB *prev = OSTCBCur; /* Remember current TCB */
	
	/* MUST Set current context to highest priority */
	OSTCBCur = OSTCBHighRdy;
	OSPrioCur = OSPrioHighRdy;
	__switch_to_asm(OSTCBHighRdy->OSTCBStkPtr, &prev->OSTCBStkPtr);
}

/* OS_Sched -> OS_TASK_SW */
void OS_TASK_SW()
{
	OSIntCtxSw();
}

/*
 * The RTOS is running in polling mode e.g. no interrupts. This approach gives a big
 * portability advantage together with less entropy @run-time but obviously we are
 * breaking the preemption model.
 *
 * Most of the architecture support the concept of free running monotonic counter which is
 * function of the CPU frequency. On X86 the TSC counter is such an example. On ARM this is
 * coming from arch_timer_read_counter()
 * In user-space similar behavior can be obtained via clock_gettime(CLOCK_MONOTONIC, &tp);
 *
 * The monotonic timebase is calibrated at intialization time. The output from calibration
 * provides "cycle_per_os_tick" which correspond to the number of CPU cycle within a timer
 * period
 *
 * UcosII preemption model EX: TimerIRQ
 * 			<<< IRQ handler
 * 		OSIntEnter();
 *			<<< Do stuff
 * 		OSTimeTick(); <<< Adjust RDY tasks
 * 		OSIntExit();
 *			OS_SchedNew(); <<< Find high ready
 *			OSIntCtxSw(); <<< OS_TASK_SW == OSIntCtxSw
 * 
 * Limitation / work-around:
 *  A) We need to call OSTimeTick manually every so often to maintain the timebase.
 * 	B) Low priority task running spinloop will block all timestamp and scheduling operation
 * 		- Easy to catch during code inspection
 * 	B) All Tasks going to sleep; Here the scheduler is _not_ involved hence we rely on OS_TaskIdle()
 * 		- Simply call OSTimeTick from OSTaskIdleHook
 * 	C) Low priority tasks doing ping-pong sem wait/pend while another High Priority task gets ready
 * 		- Here the scheduler OS_Sched() is involved for every context switch so all is needed is to
 * 		  call OSTickMonotonicTime() after every context switch.
 *
 * This function is called from OS_TaskIdle and OS_Sched. This is called right after
 * OS_EXIT_CRITICAL() as if it was an IRQ.
 *
 */

void OSTickMonotonicTime(void)
{
	u64 t;

	t = get_monotonic_cycle();

	poll_timer(t);

	tick = t;

	if( (t - prev) > cycle_per_os_tick){
		prev = t;
		OSIntEnter();
//		PRINT("###");
		OSTimeTick();
		OSIntExit(); /* OSIntCtxSw to Task ready to run */
	}
}

void OSTaskIdleHook(void)
{
	OSTickMonotonicTime();
}

#ifdef __KERNEL__
extern int main(void);
extern int (*rtos_entry)(void);
static int __init init(void)
{
    rtos_entry = main;
    return 0;
}

static void __exit fini(void)
{
    rtos_entry = NULL;
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL v2");
#endif
