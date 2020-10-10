/// P1.c - SM-08-C-4-CCS
// Contador binário acionado por S1

#include <msp430.h> 

#define TRUE    1
#define DBC     1000    //Chute para o debounce

// Protótipo das funções
void cont_leds(int ct);
void debounce(int valor);
void io_config(void);

int main(void){
   int cont=0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    io_config();
    while (TRUE){
        while ( (P2IN&BIT1) == BIT1);   //Acionou S1?
        debounce(DBC);
        cont++;
        cont_leds(cont);
        while ( (P2IN&BIT1) == 0);      //Soltou S1?
        debounce(DBC);
    }
    return 0;
}

// Acionar os leds
void cont_leds(int ct){
    switch(ct&3){
        case 0: P1OUT &= ~BIT0;     P4OUT &= ~BIT7;     break;  //00
        case 1: P1OUT &= ~BIT0;     P4OUT |=  BIT7;     break;  //01
        case 2: P1OUT |=  BIT0;     P4OUT &= ~BIT7;     break;  //10
        case 3: P1OUT |=  BIT0;     P4OUT |=  BIT7;     break;  //11
    }
}
// Debounce
void debounce(int valor){
    volatile int x;
    for (x=0; x<valor; x++);
}
// Configurar GPIO
void io_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado

    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led1 apagado

    P2DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P2REN |= BIT1;      //Habilitar resistor
    P2OUT |= BIT1;      //Habilitar pullup
}