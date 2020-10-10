// T09p07a.c
// Testar valores de captura

#include <msp430.h> 

int main(void){
    volatile unsigned int cap1,cap2,cap3;   //Capturas
    volatile unsigned int per,cc;           //Período de ciclo de carga
    volatile unsigned int pp;               //% ciclo de carga
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	cap1=40000;
	cap2=60000;
	cap3=5000;
	
    if (cap3<cap1)     per=(long) cap3 - cap1 + 65536L;
    else               per = cap3-cap1;

    if (cap2<cap1)     cc = (long) cap2 - cap1 + 65536L;
    else               cc = cap2-cap1;

    pp = 100*( (float)cc / per);
	while(1);
	return 0;
}
