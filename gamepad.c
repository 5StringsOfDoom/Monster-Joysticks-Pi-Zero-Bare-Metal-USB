#include <stdio.h>
#include <string.h>

#include "gamepad.h"
#include "rpi-gpio.h"

int16_t gpad_open(GPAD_ST* const gpad) {
	gpad->state = 0;
	RPI_SetGpioOutput( gpad->pin_strobe );
	RPI_SetGpioOutput( gpad->pin_clock );
	RPI_SetGpioInput( gpad->pin_data );
	RPI_SetGpioLo( gpad->pin_strobe );
	RPI_SetGpioLo( gpad->pin_clock );
	return 0;
}

int16_t gpad_close() {
	return -1;
}

int16_t gpad_ioctrl() {
	return -1;
}

int16_t gpad_read(GPAD_ST* const gpad) {

	int16_t i;
	RPI_SetGpioHi( gpad->pin_strobe );
	RPI_WaitMicroSeconds(2);
	RPI_SetGpioLo( gpad->pin_strobe );
	RPI_WaitMicroSeconds(2);
	gpad->state = 0;

	for (i = 0; i < 16; i++) {

		rpi_gpio_value_t curpin1 = RPI_GetGpioValue( gpad->pin_data );
		RPI_SetGpioHi( gpad->pin_clock );
		RPI_WaitMicroSeconds(2);
		RPI_SetGpioLo( gpad->pin_clock );
		RPI_WaitMicroSeconds(2);

		if (curpin1 == RPI_IO_LO) {
			gpad->state |= (1 << i);
		}
	}

	// set to 0 if the controller is not connected
	if ((gpad->state & 0xFFF) == 0xFFF) {
		gpad->state = 0;
	}

	return gpad->state;
}

