/*
 * PMM - Power Management Module Library:
 * 
 * This library contains a modified helper provided in
 * the chapter 2 of MSP430F5x User's Guide. The function
 * pmmVCore will set core voltage to a provided level. 
 * Use it with caution! One can only step-up one level 
 * at a time.
 */

#ifndef PMM_H
#define PMM_H

void pmmVCore(unsigned int level);

#endif // PMM_H
