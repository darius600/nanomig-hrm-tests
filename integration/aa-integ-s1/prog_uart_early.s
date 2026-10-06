; prog_uart_early.s -- prog_uart.s with ADKCON = $8800 in place of $0800 at step 2: UARTBRK stays set.
EARLY   equ     1
        include "prog_uart.s"
