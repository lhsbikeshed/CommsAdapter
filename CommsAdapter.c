#include "CommsAdapter.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 0,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
	uint16_t len;
	int16_t rec;
	uint8_t counter = 0;
	uint32_t lastButtonStatus = 0;
	//incoming info for leds, buffer and wait for ,
	uint8_t inBuffer[] = {0,0,0,0,0,0,0};
	uint8_t bufPtr = 0;
	uint32_t leds = 0UL;
	for (;;)
	{
		len = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
		for(counter = 0; counter < len; counter++){
			rec = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			if(rec >= 0){
				if(rec == ','){
					//buffer done
					leds =  ((unsigned long)inBuffer[0] << 24) |
						((unsigned long)inBuffer[1] << 16) |
						((unsigned long)inBuffer[2] << 8)  |
						((unsigned long)inBuffer[3]);
					bufPtr = 0;
				} else {
					inBuffer[bufPtr] = rec;
					bufPtr ++;
					if(bufPtr > 5){
						bufPtr = 0;
					}

				}
			}
		}	

		//read shift reg for keystate. Compare to last state.
		//output keys that are different to last read
		uint32_t btn = boardStatus(leds);
		if(btn != lastButtonStatus){
			for(int i = 0; i < 32; i++){
				uint8_t b = (btn & (1UL << i)) ? 1 : 0;
				uint8_t lb = (lastButtonStatus & (1UL << i)) ? 1 : 0;
				if(b != lb && b){
					char d = pgm_read_byte_near(charMap + i);
					CDC_Device_SendByte(&VirtualSerial_CDC_Interface, d);
				}
			}
			lastButtonStatus = btn;
		}

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();

	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif

	/* Hardware Initialization */
	//LEDs_Init();
	USB_Init();

	DDRD |= _BV(CLOCKPIN);
	DDRD |= _BV(READPIN);
	DDRD |= _BV(LEDSTROBE);
	DDRD |= _BV(LEDDATA);
	DDRD &= ~(_BV(DATAPIN));
	PORTD &= ~(_BV(DATAPIN));
	PORTD &= ~(_BV(CLOCKPIN));
	
}

uint32_t boardStatus(unsigned long leds){
  unsigned long buttons = 0;
  uint8_t bbit;

  //toggle the read pin
  PORTD |= _BV(READPIN);
  for(int i = 0; i < 255; i++){}
  PORTD &= ~(_BV(READPIN));

  PORTD |= _BV(LEDSTROBE);

  for (uint32_t i = 0; i < 32 ; i++) {
    //read the current bit from the keyboard

    bbit = PIND & _BV(DATAPIN);
    buttons |= ((unsigned long)bbit << i);
    //send out corresponding LED update
    PORTD |= _BV(CLOCKPIN);
    unsigned long mask = 1UL << i;
    if (leds & mask)
      PORTD &= ~(_BV(LEDDATA));
    else
      PORTD |= _BV(LEDDATA);

    PORTD &= ~(_BV(CLOCKPIN));

  }
  //  Serial.println();

  PORTD &= ~(_BV(LEDSTROBE));

  return buttons;
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

