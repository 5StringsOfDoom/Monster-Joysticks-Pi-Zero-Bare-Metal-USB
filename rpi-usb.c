#include "rpi-usb.h"

// initialize USB
void usb_init(void) {
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void) {
	return usb_configuration;
}
