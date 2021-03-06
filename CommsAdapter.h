
#ifndef _VIRTUALSERIAL_H_
#define _VIRTUALSERIAL_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <string.h>
		#include <stdio.h>
		#include <util/delay.h>

		#include "Descriptors.h"

		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Platform/Platform.h>
	
		#include <avr/pgmspace.h>

		//keyboard mapping
		PROGMEM prog_uchar charMap[] = {  
		  'a', '#', 'b', 'c', 'd', 'e', 'f', 'g', 'k', 'l', 
		  'm', 'n', 'o', 'p', 'q', 'r', '5', '4', '3', '2', 
		  '1', 'h', 'i', 'j', '6', '0', '*', '9', '8', '7', 
		  '9', '8'};
		#define CLOCKPIN PD2
		#define DATAPIN PD3
		#define READPIN PD4
		#define LEDSTROBE PD5
		#define LEDDATA PD6


	/* Function Prototypes: */
		void SetupHardware(void);
		void CheckJoystickMovement(void);
		uint32_t boardStatus(uint32_t leds);
		
		

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);

#endif

