#ifndef RPI_USB_H
#define RPI_USB_H

#include "rpi-base.h"

#define USB_BASE		( PERIPHERAL_BASE + 0x980000 )

#define USB_MDIO_CNTL	0x080
#define USB_MDIO_GEN		0x084
#define USB_VBUS_DRV		0x088


#define HW_CONFIG() (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x12)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))

void usb_init(void);			// initialize everything
uint8_t usb_configured(void);		// is the USB port configured

#endif
