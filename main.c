#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi-armtimer.h"
#include "rpi-gpio.h"
#include "rpi-interrupts.h"
#include "rpi-mailbox-interface.h"
#include "rpi-systimer.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
#define SCREEN_DEPTH    16      /* 16 or 32-bit */

#include "gamepad.h"

#define GPADSNUM 1 /* number of game pads to poll */

typedef struct {
    float r;
    float g;
    float b;
    float a;
    } colour_t;

inline void processPadBtn(uint16_t buttons, uint16_t mask) {
	if ((buttons & mask) == mask) {
		// uinput_gpad_write(uinp_gpad, key, 1, evtype);
	} else {
		// uinput_gpad_write(uinp_gpad, key, 0, evtype);
	}
}

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
	uint8_t ctr = 0;
	GPAD_ST gpads[GPADSNUM];
	int16_t clockpin;
	int16_t strobepin;
	int16_t data1pin;

	clockpin = RPI_GPIO10 ; // RPI_GPIO_P1_19;
	strobepin = RPI_GPIO11 ; // RPI_GPIO_P1_23;
	data1pin = RPI_GPIO4 ; //RPI_GPIO_P1_07;

	gpads[0].pin_clock = clockpin;
	gpads[0].pin_strobe = strobepin;
	gpads[0].pin_data = data1pin;

	gpad_open(&gpads[0]);

    int width = SCREEN_WIDTH, height = SCREEN_HEIGHT, bpp = SCREEN_DEPTH;
    int x, y, pitch = 0;
    colour_t current_colour;
    volatile unsigned char* fb = NULL;
    int pixel_offset;
    int rowBlank = (SCREEN_HEIGHT/2);
    int colBlank = (SCREEN_WIDTH/2);
    int r, g, b, a;

    /* START IN THE MIDDLE OF THE SCREEN */
    unsigned int scroll_x = 0, scroll_y = 0;

    /* Enable the timer interrupt IRQ */
    RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * 0x400 */
    RPI_GetArmTimer()->Load = 0x400;

    /* Setup the ARM Timer */
    RPI_GetArmTimer()->Control =
	RPI_ARMTIMER_CTRL_23BIT |
	RPI_ARMTIMER_CTRL_ENABLE |
	RPI_ARMTIMER_CTRL_INT_ENABLE |
	RPI_ARMTIMER_CTRL_PRESCALE_256;

    /* Enable interrupts! */
    _enable_interrupts();

    /* Print to the UART using the standard libc functions */

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_GET_BOARD_MODEL );
    RPI_PropertyAddTag( TAG_GET_BOARD_REVISION );
    RPI_PropertyAddTag( TAG_GET_FIRMWARE_VERSION );
    RPI_PropertyAddTag( TAG_GET_BOARD_MAC_ADDRESS );
    RPI_PropertyAddTag( TAG_GET_BOARD_SERIAL );
    RPI_PropertyAddTag( TAG_GET_MAX_CLOCK_RATE, TAG_CLOCK_ARM );
    RPI_PropertyProcess();

    rpi_mailbox_property_t* mp;
    mp = RPI_PropertyGet( TAG_GET_BOARD_MODEL );
    mp = RPI_PropertyGet( TAG_GET_BOARD_REVISION );
    mp = RPI_PropertyGet( TAG_GET_FIRMWARE_VERSION );
    mp = RPI_PropertyGet( TAG_GET_BOARD_MAC_ADDRESS );
    mp = RPI_PropertyGet( TAG_GET_BOARD_SERIAL );
    mp = RPI_PropertyGet( TAG_GET_MAX_CLOCK_RATE );

    /* Ensure the ARM is running at it's maximum rate */
    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_SET_CLOCK_RATE, TAG_CLOCK_ARM, mp->data.buffer_32[1] );
    RPI_PropertyProcess();

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_GET_CLOCK_RATE, TAG_CLOCK_ARM );
    RPI_PropertyProcess();
    mp = RPI_PropertyGet( TAG_GET_CLOCK_RATE );

    /* Initialise a framebuffer... */
    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_ALLOCATE_BUFFER );
    RPI_PropertyAddTag( TAG_SET_PHYSICAL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT );
    RPI_PropertyAddTag( TAG_SET_VIRTUAL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT );
    RPI_PropertyAddTag( TAG_SET_DEPTH, SCREEN_DEPTH );
    RPI_PropertyAddTag( TAG_GET_PITCH );
    RPI_PropertyAddTag( TAG_GET_PHYSICAL_SIZE );
    RPI_PropertyAddTag( TAG_GET_DEPTH );
    RPI_PropertyAddTag( TAG_SET_OVERSCAN, 0 );
    RPI_PropertyProcess();

    if( ( mp = RPI_PropertyGet( TAG_GET_PHYSICAL_SIZE ) ) )
    {
        width = mp->data.buffer_32[0];
        height = mp->data.buffer_32[1];


    }
    if( ( mp = RPI_PropertyGet( TAG_GET_DEPTH ) ) )
    {
        bpp = mp->data.buffer_32[0];
    }
    if( ( mp = RPI_PropertyGet( TAG_GET_PITCH ) ) )
    {
        pitch = mp->data.buffer_32[0];
    }
    if( ( mp = RPI_PropertyGet( TAG_ALLOCATE_BUFFER ) ) )
    {
        fb = (unsigned char*)mp->data.buffer_32[0];
    }

    /* Never exit as there is no OS to exit to! */

    while( 1 )
    {
    		// create a subroutine to poll the pad data
    		/* read states of the buttons */
    		current_colour.r = 0xFF / 2;
    	    current_colour.g = 0xFF / 2;
    	    current_colour.b = 0xFF / 2;
    	    current_colour.a = 0xFF;
    	    scroll_x = 0;
    	    scroll_y = 0;

		for (ctr = 0; ctr < GPADSNUM; ctr++) {
			gpad_read(&gpads[ctr]);
			/*processPadBtn(gpads[ctr].state, GPAD_SNES_A);*/

			/* RED */
			if( (gpads[ctr].state & GPAD_SNES_A) == GPAD_SNES_A ) {
				current_colour.r = 0xFF;
				current_colour.g = 0;
				current_colour.b = 0;
			}
			/* GREEN */
			if( (gpads[ctr].state & GPAD_SNES_Y) == GPAD_SNES_Y ) {
				current_colour.r = 0;
				current_colour.g = 0xFF;
				current_colour.b = 0;
			}
			/* BLUE */
			if( (gpads[ctr].state & GPAD_SNES_X) == GPAD_SNES_X ) {
				current_colour.r = 0;
				current_colour.g = 0;
				current_colour.b = 0xFF;
			}
			/* YELLOW */
			if( (gpads[ctr].state & GPAD_SNES_B) == GPAD_SNES_B ) {
				current_colour.r = 0xFF;
				current_colour.g = 0xFF;
				current_colour.b = 0;
			}
			/* WHITE */
			if( (gpads[ctr].state & GPAD_SNES_R) == GPAD_SNES_R ) {
				current_colour.r = 0xFF;
				current_colour.g = 0xFF;
				current_colour.b = 0xFF;
			}
			/* BLACK */
			if( (gpads[ctr].state & GPAD_SNES_L) == GPAD_SNES_L ) {
				current_colour.r = 0x80 / 2;
				current_colour.g = 0x80 / 2;
				current_colour.b = 0x80 / 2;
			}
			/* ORANGE */
			if( (gpads[ctr].state & GPAD_SNES_START) == GPAD_SNES_START ) {
				current_colour.r = 0xFF;
				current_colour.g = 0xA5;
				current_colour.b = 0x00;
			}
			/* PINK */
			if( (gpads[ctr].state & GPAD_SNES_SELECT) == GPAD_SNES_SELECT ) {
				current_colour.r = 0xFF;
				current_colour.g = 0xC0;
				current_colour.b = 0xCB;
			}
			/* PURPLE */
			if( (gpads[ctr].state & GPAD_SNES_HOME) == GPAD_SNES_HOME ) {
				current_colour.r = 0x80;
				current_colour.g = 0x00;
				current_colour.b = 0x80;
			}
			/* UP */
			if( (gpads[ctr].state & GPAD_SNES_UP) == GPAD_SNES_UP ) {
				scroll_y = -1;
			}
			/* Down */
			if( (gpads[ctr].state & GPAD_SNES_DOWN) == GPAD_SNES_DOWN ) {
				scroll_y = 1;
			}
			/* LEFT */
			if( (gpads[ctr].state & GPAD_SNES_LEFT) == GPAD_SNES_LEFT ) {
				scroll_x = -1;
			}
			/* RIGHT */
			if( (gpads[ctr].state & GPAD_SNES_RIGHT) == GPAD_SNES_RIGHT ) {
				scroll_x = 1;
			}
		}


        /* Produce a colour spread across the screen */
        for( y = 0; y < height; y++ ) {
            for( x = 0; x < width; x++ ) {
                pixel_offset = ( x * ( bpp >> 3 ) ) + ( y * pitch );

                r = (int)( current_colour.r );
                g = (int)( current_colour.g );
                b = (int)( current_colour.b );
                a = (int)( current_colour.a );

                if( x == colBlank ){
                		r = r/2;
					g = g/2;
					b = b/2;
                }

                if( y == rowBlank ){
					r = r/2;
					g = g/2;
					b = b/2;
				}

				/* Two bytes to write */
				/* Bit pack RGB565 into the 16-bit pixel offset */
				*(unsigned short*)&fb[pixel_offset] = ( (r >> 3) << 11 ) | ( ( g >> 2 ) << 5 ) | ( b >> 3 );

            }
        }
        // handle out of bpunds with rollover
        colBlank = ( colBlank > width ) ? 0 : ( colBlank < 0 ) ? width : colBlank + scroll_x ;
        rowBlank = ( rowBlank > height ) ? 0 : ( rowBlank < 0 ) ? height : rowBlank + scroll_y ;
    }
}
