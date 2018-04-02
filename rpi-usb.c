#include "rpi-usb.h"

// initialize USB
void usb_init(void) {
	HW_CONFIG();
	USB_FREEZE();				// enable USB
	PLL_CONFIG();				// config PLL
	while (!(PLLCSR & (1<<PLOCK)));		// wait for PLL lock
	USB_CONFIG();				// start USB clock
	UDCON = 0;				// enable attach resistor
	usb_configuration = 0;
	UDIEN = (1<<EORSTE)|(1<<SOFE);
	sei();
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void) {
	return usb_configuration;
}
