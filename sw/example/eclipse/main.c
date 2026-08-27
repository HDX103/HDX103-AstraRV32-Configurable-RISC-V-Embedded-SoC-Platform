#include <astrarv32.h>
#include <stdio.h>

// UART baud rate
#define BAUD_RATE 19200

// Setup the processor
void platform_init(void) {
  astrarv32_rte_setup();
  astrarv32_uart_setup(ASTRARV32_UART0, BAUD_RATE, 0);
}

// Simple busy-wait delay function
void delay_ms(uint32_t time_ms) {
  astrarv32_aux_delay_ms(astrarv32_sysinfo_get_clk(), time_ms);
}

// Main function
int main() {

  // initialize the platform
  platform_init();

  // say hello
  while(1) {
    printf("Hello Eclipse!\r\n");
    delay_ms(500);
  }

  // this should never be reached
  return 0;
}
