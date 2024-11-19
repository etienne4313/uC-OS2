#include <ucos_ii.h>

int debug = 1;

void die(int err, int line)
{
	fprintf(stderr, "DIE %d : %d\n", err, line);
	while(1){};
}

static unsigned char stack1[STACK_SIZE];
static void t1(void *p)
{
	int nr = 0;
	char *name = p;
	OS_CPU_SR cpu_sr = 0;

	while (1) {
		OS_ENTER_CRITICAL();
		PRINT("******************************%s: %d\n",name, nr);
		OS_EXIT_CRITICAL();
		nr++;
		OSTimeDlyHMSM(0,0,1,0);
	}
}

static unsigned char stack2[STACK_SIZE];
static void t2(void *p)
{
	int nr = 0;
	char *name = p;
	OS_CPU_SR cpu_sr = 0;

	while (1) {
		OS_ENTER_CRITICAL();
		PRINT("##############################%s: %d\n",name, nr);
		OS_EXIT_CRITICAL();
		nr++;
		OSTimeDlyHMSM(0,0,1,0);
	}
}

int main(void)
{
	OS_CPU_SR cpu_sr = 0;

	/* Don't let any IRQ come */
	OS_ENTER_CRITICAL();

	lib_init();
	PRINT("Entering main loop\n");

	OSInit();

	OSTaskCreate(t1, "t1", (void *)&stack1[STACK_SIZE - 1], 1);
	OSTaskCreate(t2, "t2", (void *)&stack2[STACK_SIZE - 1], 2);

	/* IRQ are enabled when the first thread is started */
	OSStart();

	/* Never reach */
	DIE(-1);
	OS_EXIT_CRITICAL();
	return 0;
}

