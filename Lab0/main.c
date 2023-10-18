#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "leds.h"
#include "spi_comm.h"
#include "motors.h"
#include "selector.h"


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
    clear_leds();
    spi_comm_start();
    motors_init();

    unsigned int s;


    /* Infinite loop. */
    while (1) {
//    	//waits 1 second
//    	// ON
//    	set_led(LED1, 1);
//        chThdSleepMilliseconds(1000);
//        // OFF
//    	set_led(LED1, 0);
//        chThdSleepMilliseconds(1000);
//    	left_motor_set_speed(0);
//    	right_motor_set_speed(0);
    	s = get_selector();

    	if (s == 0) {
    		set_led(LED1, 1);
    		set_led(LED2, 0);
    		set_led(LED3, 0);
    		set_led(LED4, 0);
    		set_led(LED5, 0);
    		set_led(LED6, 0);
    		set_led(LED7, 0);
    		set_led(LED8, 0);
    	} else if (s == 1){
    		set_led(LED1, 0);
    		set_led(LED2, 1);
    		set_led(LED3, 0);
    		set_led(LED4, 0);
    		set_led(LED5, 0);
    		set_led(LED6, 0);
    		set_led(LED7, 0);
    		set_led(LED8, 0);
    	} else {
    		set_led(LED1, 0);
    		set_led(LED2, 0);
    		set_led(LED3, 0);
    		set_led(LED4, 0);
    		set_led(LED5, 0);
    		set_led(LED6, 0);
    		set_led(LED7, 0);
    		set_led(LED8, 0);
    	}


    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
