#include <driver_init.h>

#include "rtc.h"
#include "tasks.h"

#include "control.h"

int main(void)
{
	system_init();
	task_init();
	control_init();

	while (1) {
		/* wake up after one second if scheduler does not say otherwise */
		rtc_time_t next_run = rtc_current_time + 1000;

		task_schedule(&next_run);

		/*sleep_until(next_run);*/
	}
}
