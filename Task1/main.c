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

#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"
#include "sensors/proximity.h"
#include "sensors/VL53L0X/VL53L0X.h"


const unsigned int MAX_SPEED = 1000;
const unsigned int ESTOP_SELECTOR = 0;
const unsigned int THRESHOLD = 100;

// proximity sensors initialization
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);


void init() {
  halInit();
  chSysInit();
  mpu_init();

  // LED Initialization
  clear_leds();
  spi_comm_start();

  // Motor Initialization
  motors_init();

  // Selector Initialization
  serial_start();

  // Proximity Initialization
  messagebus_init(&bus, &bus_lock, &bus_condvar);
  proximity_start(0);
  calibrate_ir();

  // TOF Initialization
  VL53L0X_start();
}

void move_forward(float speed) {
  left_motor_set_speed(speed);
  right_motor_set_speed(speed);
}

void move_backward(float speed) {
  left_motor_set_speed(-speed);
  right_motor_set_speed(-speed);
}

void turn_left(float speed) {
  left_motor_set_speed(-speed);
  right_motor_set_speed(speed);
}

void turn_right(float speed) {
  left_motor_set_speed(speed);
  right_motor_set_speed(-speed);
}

void stop() {
  left_motor_set_speed(0);
  right_motor_set_speed(0);
}

void obstacle_avoidance() {
  // left half - 4, 5, 6, 7
  // right half - 0, 1, 2, 3

  // object avoidance

  // // if object is headon, rotate right
  // if (get_calibrated_prox(0) > THRESHOLD && get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD && \
  //   get_calibrated_prox(3) > THRESHOLD && get_calibrated_prox(4) > THRESHOLD && get_calibrated_prox(5) > THRESHOLD && \
  //   get_calibrated_prox(6) > THRESHOLD && get_calibrated_prox(7) > THRESHOLD) {
  //   turn_right(MAX_SPEED);
  // }
  
  // // if object is is on left and right, but if front is clear, move forward
  // else if (get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD && get_calibrated_prox(3) > THRESHOLD && \ 
  //   get_calibrated_prox(4) > THRESHOLD && get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD && \
  //   get_calibrated_prox(7) < THRESHOLD && get_calibrated_prox(0) < THRESHOLD) {
  //   move_forward(MAX_SPEED);
    
  // // if object is on left, turn right
  // } else if (get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD && get_calibrated_prox(7) > THRESHOLD) {
  //   turn_right(MAX_SPEED);
    
  // // if object is on right, turn left
  // } else if (get_calibrated_prox(0) > THRESHOLD && get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD) {
  //   turn_left(MAX_SPEED);
  
  // // move forward
  // } else {
  //   move_forward(MAX_SPEED);
  // }


  // object avoidance
  // u side
  if (get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD  && \
    get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD && \
    get_calibrated_prox(7) > THRESHOLD && get_calibrated_prox(8) > THRESHOLD) {
    turn_right(MAX_SPEED);
    chThdSleepMilliseconds(1000); // recovery
  } else if (get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD  && \
    get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD && \
    get_calibrated_prox(7) < THRESHOLD && get_calibrated_prox(8) < THRESHOLD) {
    move_forward(MAX_SPEED);
  } else if (get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD && get_calibrated_prox(7) > THRESHOLD) {
    turn_right(MAX_SPEED);
  } else if (get_calibrated_prox(0) > THRESHOLD && get_calibrated_prox(1) > THRESHOLD && get_calibrated_prox(2) > THRESHOLD) {
    turn_left(MAX_SPEED);
  } else if (get_calibrated_prox(0) > THRESHOLD && get_calibrated_prox(1) > THRESHOLD) {
    turn_left(MAX_SPEED);
  } else if (get_calibrated_prox(5) > THRESHOLD && get_calibrated_prox(6) > THRESHOLD) {
    turn_right(MAX_SPEED);
  } else if (get_calibrated_prox(0) > THRESHOLD) {
    turn_left(MAX_SPEED);
  } else if (get_calibrated_prox(5) > THRESHOLD) {
    turn_right(MAX_SPEED);
  } else {
    move_forward(MAX_SPEED);
  }

}

void send_bt_values() {
  // send calibrated proximity values through bluetooth
  char prox_str[100];
  int str_length;
  str_length = sprintf(prox_str, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
    get_calibrated_prox(0),
    get_calibrated_prox(1),
    get_calibrated_prox(2),
    get_calibrated_prox(3),
    get_calibrated_prox(4),
    get_calibrated_prox(5),
    get_calibrated_prox(6),
    get_calibrated_prox(7),
    left_motor_get_desired_speed(),
    right_motor_get_desired_speed()
  );
  e_send_uart1_char(prox_str, str_length);    // obstacle avoidance
}



int main(void)
{
  init();

  /* Infinite loop. */
  while (1) {

    if (get_selector() == ESTOP_SELECTOR) {
      stop();
    }
    else {
      obstacle_avoidance();
      chThdSleepMilliseconds(100);
    }
  }
}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
