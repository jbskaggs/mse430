/*
 * motor.h
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#ifndef MOTOR_H_
#define MOTOR_H_

// Public variables
extern volatile signed long motor_a_count;
extern volatile signed long motor_b_count;
extern volatile int motor_a_rate;
extern volatile int motor_b_rate;

// Public function prototypes
int motor_init();
void motor_a_set_power(int power);
void motor_b_set_power(int power);
void motor_update_rates();

#endif /* MOTOR_H_ */
