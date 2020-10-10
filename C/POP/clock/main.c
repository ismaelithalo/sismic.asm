#include <msp430.h> 
#include "clock.h"
#include "pmm.h"

void main()
{
    // Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	// Increase core voltage so CPU can run at 16Mhz
	// Step 1 allows clocks up to 12MHz, step 2 allows rising MCLK to 16MHz
	pmmVCore(1);                             
	pmmVCore(2);

	// Configure clock
    // This should make MCLK @16MHz, SMCLK @1MHz and ACLK @32768Hz
	clockInit();

	// Your program here...


}
