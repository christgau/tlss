#include <stdint.h>

#include "control.h"
#include "compiler.h"

#include "rtc.h"
#include "tasks.h"
#include "communication.h"

#include "atmel_start_pins.h"

#define PIN_READY 3
#define PIN_START 2
#define PIN_FALSESTART 1

#define PIN_READY_MASK (1 << PIN_READY)
#define PIN_START_MASK (1 << PIN_START)
#define PIN_FALSESTART_MASK (1 << PIN_FALSESTART)

#define PERIOD_FALSESTART_LIGHT 250 /* ms */
#define DELAY_LIGHT_OFF_AFTER_START 15000 /* ms */
#define DELAY_SOUND_OFF_AFTER_START 1000 /* ms */

#define INT_DEBOUNCE_TIME 100 /* ms */

#define NUM_MAX_LANES 15 /* requires 3 bits configuration */

typedef enum { CT_NONE, CT_READY, CT_START, CT_FALSESTART } control_state_t;

static control_state_t state;
static rtc_time_t last_int_time[3] = { 0, 0, 0 };

static enum { OM_NONE, OM_MASTER, OM_SLAVE } operation_mode = OM_NONE;
static uint8_t lane = 0;

static struct task_handle_t *toggle_lights_task = NULL;

static void control_packet_handler(uint8_t *packet, size_t packet_length);

void control_init(void)
{
	lane = 0;
	state = CT_NONE;
	operation_mode = OM_MASTER; /* read from hardware configuration */

	communication_init(&control_packet_handler);
}

static void toggle_lights_falsestart(task_argument_t arg)
{
	TL_RED_toggle_level();
	TL_GREEN_toggle_level();

	/* TODO: toggle frequency as well */
}

static inline void sound_off(void)
{
	/* TODO: sound off */
	return;
}

static void sound_off_task(task_argument_t arg)
{
	sound_off();
}

static void reset_lights_and_sound(task_argument_t arg)
{
	state = CT_NONE;
	TL_RED_set_level(false);
	TL_GREEN_set_level(false);

	sound_off();
}

static void handle_extern_trigger(task_argument_t arg)
{
	switch (arg.byte_arg) {
		case PIN_READY:
			if (state == CT_NONE) {
				state = CT_READY;
				/* send out & do locally: turn on red light */
				TL_RED_set_level(true);
			} else {
				state = CT_NONE;
				/* send out & do locally: turn off all lights (?) */
				TL_RED_set_level(false);
				TL_GREEN_set_level(false);
			}
			break;
		case PIN_START:
			if (state == CT_READY) {
				state = CT_START;
				/* send out & do locally: turn off green, turn on red light, make beep (turn of via task) */
				TL_RED_set_level(false);
				TL_GREEN_set_level(true);
				task_create(&sound_off_task, NULL, TASK_IN(DELAY_SOUND_OFF_AFTER_START));
				task_create(&reset_lights_and_sound, NULL, TASK_IN(DELAY_LIGHT_OFF_AFTER_START));
			}
			/* do nothing more! */
			break;
		case PIN_FALSESTART:
			if (state == CT_READY || state == CT_START) {
				state = CT_FALSESTART;
				/* send out & do locally: make traffic light panic */
				toggle_lights_task = task_create(&toggle_lights_falsestart, NULL, TASK_PERIODICAL(PERIOD_FALSESTART_LIGHT));
			} else if (state == CT_FALSESTART)  {
				/* send out & do locally: turn everything off */
				task_destroy(toggle_lights_task);
				task_create(&reset_lights_and_sound, NULL, TASK_NOW);
			}
			break;
		default:
			break;
			/* nop */
	}
}

ISR(PORTA_PORT_vect)
{
	task_argument_t arg;

    /* Do the same things in master and slave mode. In slave mode this can be considered for a wired interface. */
	if (operation_mode != OM_NONE) {
		/* Schedule one (!) task according to incoming interrupt. */
		if ((VPORTA_INTFLAGS & PIN_READY_MASK) && (rtc_current_time - last_int_time[0]) >= INT_DEBOUNCE_TIME) {
			arg.byte_arg = PIN_READY;
			task_create(&handle_extern_trigger, &arg, 0);
		} else if ((VPORTA_INTFLAGS & PIN_START_MASK) && (rtc_current_time - last_int_time[1]) >= INT_DEBOUNCE_TIME) {
			arg.byte_arg = PIN_START;
			task_create(&handle_extern_trigger, &arg, 0);
		} else if ((VPORTA_INTFLAGS & PIN_FALSESTART_MASK) && (rtc_current_time - last_int_time[2]) >= INT_DEBOUNCE_TIME) {
			arg.byte_arg = PIN_FALSESTART;
			task_create(&handle_extern_trigger, &arg, 0);
		}
	}

	/* Clear interrupt flags */
	VPORTA_INTFLAGS = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7);
}

static void control_packet_handler(uint8_t *packet, size_t packet_length)
{
	/* TODO: handle the packet */
}
