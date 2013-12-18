/*****************************************************************************
 *   uart.h:  Header file for NXP LPC13xx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.21  ver 1.00    Preliminary version, first Release
 *   2013.11.01	 ver 1.10	 Jeffrey Nelson - Added additional convenience functions
 *
******************************************************************************/
#ifndef __UART_H 
#define __UART_H
#include <stdbool.h>

#define RS485_ENABLED		0
#define TX_INTERRUPT		0		/* 0 if TX uses polling, 1 interrupt driven. */
#define MODEM_TEST			0

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define BUFSIZE		0x0 // Saved some ram for Lasershark haha.

/* RS485 mode definition. */
#define RS485_NMMEN		(0x1<<0)
#define RS485_RXDIS		(0x1<<1)
#define RS485_AADEN		(0x1<<2)
#define RS485_SEL		(0x1<<3)
#define RS485_DCTRL		(0x1<<4)
#define RS485_OINV		(0x1<<5)


void UARTInit(uint32_t Baudrate);

// These two will probably be removed.. just don't want to delete them yet.
//void UART_IRQHandler(void);
//void UARTSend(uint8_t *BufferPtr, uint32_t Length);

__inline bool uart_rx_ready();
 uint8_t uart_rx_nonblock(uint8_t *byte_array, uint8_t byte_count);
 uint8_t uart_tx_nonblock(uint8_t *byte_array, uint8_t byte_count);
 void uart_rx_clear_fifo();




#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
