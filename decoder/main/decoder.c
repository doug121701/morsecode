/**
 * @file decoder.c
 * @author Douglas Rawcliffe, rawcliffedouglas@gmail.com
 * @brief Decodes a morese code signal from a light source.
 * 		  Uses a light detector sautered onto an ESP32C3 board.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>


#define ADC_CHANNEL ADC_CHANNEL_0
#define THRESHOLD 40 // The min light level to be deted
#define SHORT 20 // How long a short pulse should be
#define LONG 50 // how long a long pulse should be
#define MORSE_SPACE 70 
#define END_LENGTH 6000
#define MAX_NOISE 5
static const char* TAG = "light detector";

//Define the morse codes, 1 is a ., 2 is a -, etc
#define A_CODE 12
#define B_CODE 2111
#define C_CODE 2121
#define D_CODE 211
#define E_CODE 1
#define F_CODE 1121
#define G_CODE 221
#define H_CODE 1111
#define I_CODE 11
#define J_CODE 1222
#define K_CODE 212
#define L_CODE 1211
#define M_CODE 22
#define N_CODE 21
#define O_CODE 222
#define P_CODE 1221
#define Q_CODE 2212
#define R_CODE 121
#define S_CODE 111
#define T_CODE 2
#define U_CODE 112
#define V_CODE 1112
#define W_CODE 122
#define X_CODE 2112
#define Y_CODE 2122
#define Z_CODE 2211
#define ZERO_CODE 22222
#define ONE_CODE 12222
#define TWO_CODE 11222
#define THREE_CODE 11122
#define FOUR_CODE 11112
#define FIVE_CODE 11111
#define SIX_CODE 21111
#define SEVEN_CODE 22111
#define EIGHT_CODE 22211
#define NINE_CODE 22221
#define END_CODE 22122
#define SPACE_CODE 1212

// Decodes a code into a character
char decode(int t){
	switch(t){
		case SPACE_CODE:
			return ' ';
		case A_CODE:
			return 'a';
		case B_CODE:
            return 'b';
		case C_CODE:
            return 'c';
		case D_CODE:
            return 'd';
		case E_CODE:
            return 'e';
		case F_CODE:
            return 'f';
		case G_CODE:
            return 'g';
		case H_CODE:
            return 'h';
		case I_CODE:
            return 'i';
		case J_CODE:
		    return 'j';
		case K_CODE:
            return 'k';
		case L_CODE:
            return 'l';
		case M_CODE:
    		return 'm';
		case N_CODE:
        	return 'n';
		case O_CODE:
            return 'o';
		case P_CODE:
            return 'p';
		case Q_CODE:
            return 'q';
		case R_CODE:
            return 'r';
		case S_CODE:
            return 's';
		case T_CODE:
            return 't';
		case U_CODE:
            return 'u';
		case V_CODE:
            return 'v';
		case W_CODE:
            return 'w';
		case X_CODE:
            return 'x';
		case Y_CODE:
            return 'y';
		case Z_CODE:
            return 'z';
		case ONE_CODE:
            return '1';
		case TWO_CODE:
            return '2';
		case THREE_CODE:
            return '3';
		case FOUR_CODE:
            return '4';
		case FIVE_CODE:
            return '5';
		case SIX_CODE:
            return '6';
		case SEVEN_CODE:
            return '7';
		case EIGHT_CODE:
            return '8';
		case NINE_CODE:
            return '9';
		case ZERO_CODE:
            return '0';
		case END_CODE:
			return '\0';
	}
	// Invalid code
	return '?';
}

void app_main(void){
	// Codigure the channel on which on the sensor is
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
	
	int last_state = 0; // What state the sensor is in 0, no light, 1 light
	int pulse_duration = 0; // How long the pulse is
	int letter_started = 0;
	int space_detected = 0;

	char detected_message[200]; // 200 limit for message
	int message_index = 0;

	int code = 0;
	int noise = 0;
	// Continously search signals
	while(1){
		int val = adc1_get_raw(ADC_CHANNEL);
	
		int current_state = (val > THRESHOLD) ? 1 : 0;

		//Change in state, light went on to off, or off to on
		if (current_state != last_state) {
			//Light went on to off
			//Pulse finished
			if(last_state == 1){
				//Long pulse, '-'
				if(pulse_duration >= LONG && !space_detected){
					ESP_LOGI(TAG, "Long pulse detected");
					code = code * 10 + 2;
				}
				//Short pulse, '.'
				else if (pulse_duration >= SHORT && !space_detected){
					ESP_LOGI(TAG, "Short pulse detected");
					code = code * 10 + 1;
				}
				pulse_duration = 0;
				letter_started = 1;
				space_detected = 0;
			}
			//just started pulse
			else{
				pulse_duration = 1;
			}
		}
		//No change
		else{
			if (current_state == 1){
				pulse_duration += 10; // +10 since we read every 10 ms
			}
		}

		last_state = current_state;

		//If there is a space detected
		if (pulse_duration >= MORSE_SPACE && letter_started && !space_detected){
			ESP_LOGI(TAG, "space detected");
			space_detected = 1;
			char c = decode(code);
			ESP_LOGI(TAG, "C: %c, Code: %d, Ind: %d\n", c, code, message_index);
			//sucessfully decoded
			code = 0;
			if (c != '?'){
				detected_message[message_index] = c;
				//printf("MESS at %d: %c\n", message_index, detected_message[message_index]);
				message_index += 1;
			}
			//End of message
			if (c == '\0'){
				printf("Message: %s\n", detected_message);
				for (int i = 0; i < 200; i++){
					//printf("m[i]: %c\n", detected_message[i]);
					detected_message[i] = '\0';
				}
				message_index = 0;
			}
		}
		
		/*if(val >= THRESHOLD){
			printf("Raw: %d\n", val);
		}*/

		// Read every 10 ms
		vTaskDelay(pdMS_TO_TICKS(10));
	}

}
