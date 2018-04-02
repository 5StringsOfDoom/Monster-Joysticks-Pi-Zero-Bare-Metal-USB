#ifndef GAMEPAD_H_
#define GAMEPAD_H_

/* bit masks for checking the button states for SNES controllers */
#define GPAD_SNES_B       0x01
#define GPAD_SNES_Y       0x02
#define GPAD_SNES_SELECT  0x04
#define GPAD_SNES_START   0x08
#define GPAD_SNES_UP      0x10
#define GPAD_SNES_DOWN    0x20
#define GPAD_SNES_LEFT    0x40
#define GPAD_SNES_RIGHT   0x80
#define GPAD_SNES_A       0x100
#define GPAD_SNES_X       0x200
#define GPAD_SNES_L       0x400
#define GPAD_SNES_R       0x800
#define GPAD_SNES_HOME    0x1000

/* holds the GPIO pins for the clock, strobe and data signals */
typedef struct {
	int16_t pin_clock;
	int16_t pin_strobe;
	int16_t pin_data;
	uint16_t state;
} GPAD_ST;

int16_t gpad_open(GPAD_ST* const gpad);
int16_t gpad_close();
int16_t gpad_ioctrl();
int16_t gpad_read(GPAD_ST* const gpad);

#endif
