//=============================================================================
// MMI.c                                                           20130620 CHG
//
//=============================================================================
// 
//-----------------------------------------------------------------------------
#include "chip.h"
#include "System.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "MMI.h"

typedefConfig Config;


#define LPC824MAX // define this if running on LPC824 MAX Expresso board

//---------------------------------------------------------------------------------------
// I/O signals
//---------------------------------------------------------------------------------------
#ifdef LPC824MAX
	// LPC824 MAX board
	#define PORT_BUTTON_PIN			0	// Port number for button
	#define PIN_S1              	4	// P0.4 Button
	#define PIN_S1_MODE IOCON_PIO4  	// Used for setting of pullup etc

	#define PORT_LED_PIN			0	// Port number for LEDs
	#define PIN_DBG_LED_RED    		12	// P0.12 Diagnostic red LED
	#define PIN_DBG_LED_YELLOW 		16	// P0.16 Diagnostic yellow LED
	#define PIN_DBG_LED_BLUE   		27	// P0.27 Diagnostic blue LED
#else
	// Real hardware
	#define PORT_BUTTON_PIN		 	0	// Port number for button
	#define PIN_S1              	15	// P0.15 Button
	#define PIN_S1_MODE IOCON_PIO15   	// Used for setting of pullup etc

	#define PORT_LED_PIN	  		0	// Port number for LEDs
	#define PIN_DBG_LED_RED     	8	// P0.08 Diagnostic red LED
	#define PIN_DBG_LED_YELLOW  	9	// P0.09 Diagnostic yellow LED
	#define PIN_DBG_LED_BLUE    	1	// P0.01 Diagnostic blue LED
#endif

//---------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------
typedefConfig *getConfigMMI(void) {
	return &Config;
}

//---------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------
void ledMMI(int num, int state) {
  switch (num) {

    case LED_DBG_RED:
      if (state==LED_OFF)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_RED, TRUE);
      else if (state==LED_ON)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_RED, FALSE);
      else 
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_RED, !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_RED));
      break;
	  
    case LED_DBG_YELLOW:
      if (state==LED_OFF)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_YELLOW, TRUE);
      else if (state==LED_ON)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_YELLOW, FALSE);
      else 
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_YELLOW, !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_YELLOW));
      break;
	  
    case LED_DBG_BLUE:
      if (state==LED_OFF)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_BLUE, TRUE);
      else if (state==LED_ON)
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_BLUE, FALSE);
      else 
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_BLUE, !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, PORT_LED_PIN, PIN_DBG_LED_BLUE));
      break;
	  
  }
}

//----------------------------------------------------------------------------
// Return state of button S1
//----------------------------------------------------------------------------
int buttonMMI(void) {
	return Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_BUTTON_PIN, PIN_S1) ? 0:1;
}


//---------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------
void initMMI(void) {
  	// Set LED port pins to output direction
	Chip_GPIO_SetPortDIROutput(LPC_GPIO_PORT,PORT_LED_PIN, (1<<PIN_DBG_LED_RED) | (1<<PIN_DBG_LED_YELLOW) | (1<<PIN_DBG_LED_BLUE));
	ledMMI(LED_DBG_RED, FALSE);
	ledMMI(LED_DBG_YELLOW, FALSE);
	ledMMI(LED_DBG_BLUE, FALSE);

	// Set pullup for button
	Chip_IOCON_PinSetMode(LPC_IOCON, PIN_S1_MODE, PIN_MODE_PULLUP);

	//Chip_EEPROM_Read(0x0000, (unsigned char*)&Config, sizeof(Config));
	// Check if record is correct
	if (Config.cookie != CONFIG_COOKIE) {
		// Not valid, go set config record to all 0's
		memset(&Config, 0, sizeof(Config));
		Config.enableStatistics=0;
	}

}
