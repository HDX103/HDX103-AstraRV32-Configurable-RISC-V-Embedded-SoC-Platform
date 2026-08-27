// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file demo_clint/main.c
 * @brief Simple core local interruptor (CLINT) usage example.
 **************************************************************************/

#include <astrarv32.h>

// UART BAUD rate
#define BAUD_RATE 19200

// Prototypes
void mti_irq_handler(void);
void msi_irq_handler(void);

// Week day list
const char weekdays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};


/**********************************************************************//**
 * This program toggles an LED at GPIO.output(0) at 1Hz and also prints and updates
 * the Unix time in human-readable format using the machine timer interrupt.
 *
 * @note This program requires the CLINT to be synthesized (and UART0 and GPIO).
 *
 * @return Should not return.
 **************************************************************************/
int main() {

  // capture all exceptions and give debug info via UART
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);

  // check if CLINT unit is implemented at all
  if (astrarv32_clint_available() == 0) {
    astrarv32_uart0_puts("[ERROR] CLINT not implemented!\n");
    return 1;
  }
  // check if GPIO module is implemented at all
  if (astrarv32_gpio_available() == 0) {
    astrarv32_uart0_puts("[ERROR] GPIO module not implemented!\n");
    return 1;
  }

  // Intro
  astrarv32_uart0_puts("RISC-V Core-Local Interruptor (CLINT) demo Program.\n"
                     "Real-time clock using the RISC-V MTIMER interrupt.\n"
                     "Also toggles GPIO.output(0) at 1Hz.\n\n");

  // clear GPIO output port

  // configure lowest 8 GPIO pins as outputs
  astrarv32_gpio_port_set(0);
  astrarv32_gpio_dir_set(1); // pin 0 = output

  // setup date and time for the Unix time of CLINT.MTIMER
  date_t date;
  date.year    = 2025; // current year (absolute)
  date.month   = 10;   // 1..12
  date.day     = 24;   // 1..31
  date.hours   = 23;   // 0..23
  date.minutes = 01;   // 0..59
  date.seconds = 17;   // 0..59

  astrarv32_clint_unixtime_set(astrarv32_aux_date2unixtime(&date));
  astrarv32_uart0_printf("Unix timestamp: %u\n", (uint32_t)astrarv32_clint_unixtime_get());

  // configure MTIME timer to not trigger
  astrarv32_clint_mtimecmp_set(-1);

  // install CLINT handlers to RTE
  astrarv32_rte_handler_install(TRAP_CODE_MTI, mti_irq_handler);
  astrarv32_rte_handler_install(TRAP_CODE_MSI, msi_irq_handler);

  // start real time clock
  astrarv32_uart0_printf("\nStarting real-time clock demo...\n");

  // configure MTIME timer's first interrupt to trigger after 1 second starting from now
  astrarv32_clint_mtimecmp_set(astrarv32_clint_time_get() + astrarv32_sysinfo_get_clk());

  // enable machine time and software interrupts
  astrarv32_cpu_csr_set(CSR_MIE, (1 << CSR_MIE_MTIE) + (1 << CSR_MIE_MSIE));
  astrarv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

  // go to sleep mode and wait for interrupt
  while(1) {
    astrarv32_cpu_sleep();
  }

  return 0;
}


/**********************************************************************//**
 * Machine timer IRQ handler.
 *
 * @warning This function has to be of type "void xyz(void)" and must not use any interrupt attributes!
 **************************************************************************/
void mti_irq_handler(void) {

  // configure MTIME timer's next interrupt to trigger after 1 second MTIMECMP delta
  astrarv32_clint_mtimecmp_set(astrarv32_clint_mtimecmp_get() + astrarv32_sysinfo_get_clk());

  // toggle output port bit 0
  astrarv32_gpio_pin_toggle(0);

  // trigger software interrupt for this core (just for fun)
  astrarv32_clint_msi_set(astrarv32_cpu_csr_read(CSR_MHARTID));

  // show date in human-readable format
  date_t date;
  astrarv32_aux_unixtime2date(astrarv32_clint_unixtime_get(), &date);
  astrarv32_uart0_printf("%u.%u.%u (%s) ", date.day, date.month, date.year, weekdays[(date.weekday-1)%7]);
  astrarv32_uart0_printf("%u:%u:%u\n", date.hours, date.minutes, date.seconds);
}


/**********************************************************************//**
 * Machine software IRQ handler.
 *
 * @warning This function has to be of type "void xyz(void)" and must not use any interrupt attributes!
 **************************************************************************/
void msi_irq_handler(void) {

  // clear machine software interrupt for this core
  astrarv32_clint_msi_clr(astrarv32_cpu_csr_read(CSR_MHARTID));

  astrarv32_uart0_printf("\n[Machine Software Interrupt!]\n");
}
