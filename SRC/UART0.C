#include "chip.h"
#include "String.h"
#include "uart0.h"

#define LPC_USART       LPC_USART0
#define LPC_IRQNUM      UART0_IRQn
#define LPC_UARTHNDLR   UART0_IRQHandler


static int tByte=0;

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
__attribute__ ((weak)) void rxCBUART0(unsigned char ch) {
  ;//do nothing if user doesn't overload
}


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void LPC_UARTHNDLR(void)
{
	/* New data will be ignored if data not popped in time */
	while ((Chip_UART_GetStatus(LPC_USART) & UART_STAT_RXRDY) != 0) {
		uint8_t ch = Chip_UART_ReadByte(LPC_USART);
		rxCBUART0(ch);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void initUART0(int baudrate) {
	Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 0);  // P0.0 is RxD
	Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 14); // P0.14 is TxD

	Chip_UART_Init(LPC_USART);
	
	Chip_UART_ConfigData(LPC_USART, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((baudrate * 16), true);
	Chip_UART_SetBaud(LPC_USART, baudrate);
	Chip_UART_Enable(LPC_USART);
	Chip_UART_TXEnable(LPC_USART);

	// Enable receive data and line status interrupt
	Chip_UART_IntEnable(LPC_USART, UART_INTEN_RXRDY);

	// Enable UART interrupt 
	NVIC_EnableIRQ(LPC_IRQNUM);
}


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void sendUART0(unsigned char* txbuf, int buflen) {
 	Chip_UART_SendBlocking(LPC_USART, txbuf, buflen);
}
