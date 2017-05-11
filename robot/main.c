/*
 * motor.c
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "mse430.h"
#include "uart.h"
#include "motor.h"
#include "speed.h"
#include "commands.h"

union message {
	char bytes[4];
	struct {
		int arg_a;
		int arg_b;
	} command;
} message;

int main(void) {

	// Initialize everything
	mse430_init();

	// Loop forever -- this is a priority scheduler, so higher listed
	// tasks will always take precedence over and can starve lower
	// tasks. We aren't doing much here so it's okay. This may go to
	// sleep if a flag was set just after it checked, but for
	// simplicity I'm leaving that for now. It wakes up 64 times a
	// second anyway, so it'll catch it eventually. (TODO?)
	while (1) {

		__disable_interrupt();

		if (tick_flag) {
			__enable_interrupt();

			// Run speed controller
			tick_flag = 0;
			if (controller_on)
			    speed_controller_tick();

			// Send speed back to remote
			message.command.arg_a = motor_a_rate;
			message.command.arg_b = motor_b_rate;
			//IOputc('S', uart_tx_buf);
			IOnputs(message.bytes, 4, uart_tx_buf);

		} else if (uart_rx_buf->count >= 5) {
			__enable_interrupt();

			// Process a command from the remote
			char c;
			IOgetc(&c, uart_rx_buf);
			switch (c) {
			case 'P':
				IOgetc(&message.bytes[0], uart_rx_buf);
				IOgetc(&message.bytes[1], uart_rx_buf);
				IOgetc(&message.bytes[2], uart_rx_buf);
				IOgetc(&message.bytes[3], uart_rx_buf);
				motor_a_set_power(message.command.arg_a);
				motor_b_set_power(message.command.arg_b);
				break;
			case 'S':
				IOgetc(&message.bytes[0], uart_rx_buf);
				IOgetc(&message.bytes[1], uart_rx_buf);
				IOgetc(&message.bytes[2], uart_rx_buf);
				IOgetc(&message.bytes[3], uart_rx_buf);
				speed_a_set_target(message.command.arg_a);
				speed_b_set_target(message.command.arg_b);
				break;
			default:
				break;
			}

		} else {
			__bis_SR_register(LPM0_bits | GIE);

		}
	}
}
