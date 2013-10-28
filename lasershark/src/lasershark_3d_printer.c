#include "lasershark_3d_printer.h"
#include "lasershark.h"
#include "gpio.h"
#include "timer32.h"
#include <string.h>

static uint32_t lasershark_3d_printer_stepper_1_home_dir;
static uint32_t lasershark_3d_printer_stepper_2_home_dir;
static uint32_t lasershark_3d_printer_stepper_step_delay_ms;

void lasershark_3dprinter_init() {
	GPIOSetDir(LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN, 1);
	GPIOSetDir(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN, 1);

	GPIOSetDir(LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PIN, 1);
	GPIOSetDir(LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PIN, 1);

	GPIOSetDir(LASERSHARK_3D_PRINTER_R1_PORT, LASERSHARK_3D_PRINTER_R1_PIN, 1);
	GPIOSetDir(LASERSHARK_3D_PRINTER_R2_PORT, LASERSHARK_3D_PRINTER_R2_PIN, 1);

	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN, 0);
	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN, 0);

	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PIN, 0);
	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PORT,
			LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PIN, 0);

	GPIOSetDir(LASERSHARK_3D_PRINTER_R1_PORT, LASERSHARK_3D_PRINTER_R1_PIN, 0);
	GPIOSetDir(LASERSHARK_3D_PRINTER_R2_PORT, LASERSHARK_3D_PRINTER_R2_PIN, 0);


	lasershark_3d_printer_stepper_1_home_dir = 0;
	lasershark_3d_printer_stepper_2_home_dir = 0;
	lasershark_3d_printer_stepper_step_delay_ms = 1;

	init_timer32(0, 0);

}


static __INLINE uint8_t lasershark_3d_printer_get_r1_state() {
	return GPIOGetValue( LASERSHARK_3D_PRINTER_R1_PORT, LASERSHARK_3D_PRINTER_R1_PIN);

}

static __INLINE uint8_t lasershark_3d_printer_get_r2_state() {
	return GPIOGetValue( LASERSHARK_3D_PRINTER_R2_PORT, LASERSHARK_3D_PRINTER_R2_PIN);

}


static bool lasershark_3d_printer_stepper_step_helper(uint32_t dir_port, uint32_t dir_pin, uint32_t step_port, uint32_t step_pin, uint32_t dir, uint32_t steps)
{
	int i;
	GPIOSetValue(dir_port, dir_pin, dir);
	delay32Ms(1, lasershark_3d_printer_stepper_step_delay_ms);
	for (i = 0; i < steps; i++) { // High steps cause step...
		GPIOSetValue(step_port, step_pin, 0);
		delay32Ms(0, lasershark_3d_printer_stepper_step_delay_ms);
		GPIOSetValue(step_port, step_pin, 1);
		delay32Ms(0, lasershark_3d_printer_stepper_step_delay_ms);
	}
	return true;
}


static bool lasershark_3d_printer_stepper_home(uint32_t steppers)
{

	uint8_t r1, r2;
	if (steppers & ~(LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME_STEPPER_1 | LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME_STEPPER_2))
	{
		return false;
	}

	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN, lasershark_3d_printer_stepper_1_home_dir);
	GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PIN, lasershark_3d_printer_stepper_2_home_dir);
	delay32Ms(1, lasershark_3d_printer_stepper_step_delay_ms);

	r1 = lasershark_3d_printer_get_r1_state();
	r2 = lasershark_3d_printer_get_r2_state();
	while (r1 || r2) {
		if (r1) {
			GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN, 0);
		}
		if (r2) {
			GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PIN, 0);
		}
		delay32Ms(0, lasershark_3d_printer_stepper_step_delay_ms);

		if (r1) {
			GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN, 1);
		}
		if (r2) {
			GPIOSetValue(LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PIN, 1);
		}
		delay32Ms(0, lasershark_3d_printer_stepper_step_delay_ms);

		r1 = lasershark_3d_printer_get_r1_state();
		r2 = lasershark_3d_printer_get_r2_state();
	}


	return true;
}






static bool lasershark_3d_printer_set_stepper_1_home_dir(uint8_t dir) {
	if (dir != 1 && dir != 0) {
		return false;
	}

	lasershark_3d_printer_stepper_1_home_dir = dir;
	return true;
}

static bool lasershark_3d_printer_set_stepper_2_home_dir(uint8_t dir) {
	if (dir != 1 && dir != 0) {
		return false;
	}

	lasershark_3d_printer_stepper_2_home_dir = dir;
	return true;
}

// Returns 1 on failure, 0 otherwise.
static bool laser_3d_printer_stepper_set_step_delay_ms_cmd(uint32_t ms) {
	if ((ms < LASERSHARK_3D_PRINTER_STEPPER_STEP_DELAY_MS_MIN
			|| ms > LASERSHARK_3D_PRINTER_STEPPER_STEP_DELAY_MS_MAX)) {
		return false;
	}

	lasershark_3d_printer_stepper_step_delay_ms = ms;
	return 1;
}


void lasershark_process_3d_printer_command() {
	uint32_t temp;

	// The command sent has already been put in the "IN" buffer
	// The output is assumed to be a success report.

	switch (OUT1Packet[0]) {
	case LASERSHARK_3D_PRINTER_CMD_STEPPER_1_STEP_TOWARDS_HOME:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_stepper_step_helper(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN,
				LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN,
				lasershark_3d_printer_stepper_1_home_dir, temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_STEPPER_1_STEP_AWAY_FROM_HOME:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_stepper_step_helper(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN,
				LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN,
				!lasershark_3d_printer_stepper_1_home_dir, temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_STEPPER_2_STEP_TOWARDS_HOME:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_stepper_step_helper(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN,
				LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN,
				lasershark_3d_printer_stepper_1_home_dir, temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_STEPPER_2_STEP_AWAY_FROM_HOME:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_stepper_step_helper(LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT, LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN,
				LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT,
				LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN,
				!lasershark_3d_printer_stepper_1_home_dir, temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_stepper_home(temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_GET_R1:
		temp = lasershark_3d_printer_get_r1_state();
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_3D_PRINTER_CMD_GET_R2:
		temp = lasershark_3d_printer_get_r2_state();
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_3D_PRINTER_CMD_SET_STEPPER_1_HOME_DIR:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_set_stepper_1_home_dir(temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_CMD_SET_STEPPER_2_HOME_DIR:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_3d_printer_set_stepper_2_home_dir(temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_3D_PRINTER_STEPPER_SET_SET_DELAY_MS:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!laser_3d_printer_stepper_set_step_delay_ms_cmd(temp)) {
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	default:
		IN1Packet[1] = LASERSHARK_CMD_UNKNOWN;
		break;
	}
}
