//----------------------------------------------------------------------------
// Main.c                                                         20151016 CHG
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "chip.h"
#include "system.h"
#include "MMI.h"
#include "UART0.h"
#include "Firmware.h"
#define OS_TICK 10000 // RTOS tick in uSec

//-----------------------------------------------------------------------------
// Version number
int FWVersion=10;  // divided with 100, value 123 is version 1.23
int FWTarget=1;	   // Target ()
//-----------------------------------------------------------------------------
// Changelog:
// 
// 0.10 20151101
//      First version
//-----------------------------------------------------------------------------




// System oscillator rate and clock rate on the CLKIN pin
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;


// Stack for threads
unsigned long long stkthMain[160];  


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void rxCBUART0(unsigned char ch) {
	ledMMI(LED_DBG_YELLOW, LED_TOGGLE);
}

//----------------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------------
static __asm void BootJump(unsigned int firmwareStartAddress) {
	LDR R1, [R0]
	MSR MSP, R1    	 ; Load MSP with new stack pointer address

	LDR R1, [R0]     ;Load new stack pointer address
	MOV SP, R1     	 ;Load new stack pointer address

	LDR R1, [R0, #4] ;Load new program counter address
;	MOV PC, R1 		 ;Load new program counter address
	BX  R1 		 	;Load new program counter address
}


//-----------------------------------------------------------------------------
// Main thread
//-----------------------------------------------------------------------------
 void thMain (void) {
	volatile int freq, tick, status;
	 
	SystemCoreClockUpdate();
	 
	// Configure RTOS tick to be 10 mSec (calculated from actual system clock)
	SysTick->LOAD=((U32)(((double)Chip_Clock_GetSystemClockRate()*(double)OS_TICK)/1E6)-1);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	// 824 needs the fixed pin ACMP2 pin disabled to use pin as gpio
	Chip_SWM_DisableFixedPin(SWM_FIXED_CLKIN);



	// Enable the power to the WDT 
	Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_WDTOSC_PD);
	// Freq = 0.6Mhz, divided by 64. WDT_OSC should be 9.375khz
	Chip_Clock_SetWDTOSC(WDTLFO_OSC_0_60, 64);
	// The WDT divides the input frequency into it by 4 
	freq = Chip_Clock_GetWDTOSCRate() / 4;	 
	// (wdtFreq is now 2343 Hz)

	// Check if WDT caused the reset, if so increment counter
	status=Chip_WWDT_GetStatus(LPC_WWDT);
	if (status & WWDT_WDMOD_WDTOF) {
		int restart;
		restart=Chip_PMU_ReadGPREG(LPC_PMU, 0);
		restart++;
		Chip_PMU_WriteGPREG(LPC_PMU, 0, restart);
	}

	// Initialize WWDT (also enables WWDT clock)
	Chip_WWDT_Init(LPC_WWDT);
	// Configure WWDT to reset on timeout and lock clocksource so it continues to run in powerdown mode
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET | (1<<5));
	// Set timeout to approx 2 seconds
	Chip_WWDT_SetTimeOut(LPC_WWDT, freq * 10); 
	// Start WDT
	Chip_WWDT_Start(LPC_WWDT);
	initMMI();
	OS_WAIT(1000);
 
	// If button is pressed, do NOT start firmware
	// otherwise start firmware if present
	if (buttonMMI()==FALSE) {
		if(*(unsigned int*)FW_ENTRY_CM0P != 0xFFFFFFFF) {
			// Disable all interrupts
			NVIC->ICER[0] = 0xFFFFFFFF;
			// Clear all pending interrupts 
			NVIC->ICPR[0] = 0xFFFFFFFF;
			tsk_lock();
			// IRQ vectors at BASE of firmware
			SCB->VTOR =  FW_ENTRY_CM0P & 0x1FFFFF80; 
			// Lets go..
			BootJump(FW_ENTRY_CM0P);
		}
	}



	initUART0(115200);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	
	//-----------------------------------------------------------------------------
	// The never ending story..
	//-----------------------------------------------------------------------------
	while (1) {
		tick++;

		Chip_WWDT_Feed(LPC_WWDT);

		ledMMI(LED_DBG_RED, LED_ON);
		OS_WAIT(300);
		ledMMI(LED_DBG_RED, LED_OFF);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_ON);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_OFF);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_ON);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_OFF);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_ON);
		OS_WAIT(100);
		ledMMI(LED_DBG_RED, LED_OFF);

		OS_WAIT(1000);
		
	}
}


//-----------------------------------------------------------------------------
// Main, initialize RTOS and start main thread
//-----------------------------------------------------------------------------
int main (void) {
	// Start main thread (we will never return here again)
	INIT_THREAD_USR(thMain, 1, &stkthMain, sizeof(stkthMain));
}

