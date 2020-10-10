/*
 * UCS - Unified Clock System Library
 * 
 * This library provides a helper for setting system clocks 
 * to a pre-defined value. The function clockInit(), will
 * connect the launchpad crystals to the UCS module and use 
 * them as reference to set MCLK @16Hz, SMCLK @1MHz and 
 * ACLK @32768Hz. 
 */

#ifndef CLOCK_H_
#define CLOCK_H_

// This macro makes configuration of FLLN more intuitive.
#define FLLN(x) ((x)-1)

// Initialize MCLK @16MHz, SMCLK @1MHz and ACLK @32768Hz
void clockInit();

#endif /* CLOCK_H_ */
