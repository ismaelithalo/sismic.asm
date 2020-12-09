// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// B2p4-Esqueleto.C
//
// USCI_A0   P3.3 (TXD) ===> P4.3 (RXD) USCI_A1
//           P3.4 (RXD) <=== P4.0 (TXD)

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

void USCI_A0_config(void);
void USCI_A1_config(void);
void gpio_config(void);

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    gpio_config();
    USCI_A0_config();
    USCI_A1_config();

    while(TRUE){
        //Coloque seu c�digo dentro deste la�o
        ;
    }
    return 0;
}

// ISR da USCI_A1
#pragma vector = USCI_A1_VECTOR         //Vetor 46
__interrupt void isr_USCI_A1(void){;}

// Configurar USCI_A0 para 19.200
void USCI_A0_config(void){;}

// Configurar USCI_A1 para 19.200
void USCI_A1_config(void){
    ;
    PMAPKEYID = 0X02D52;   //Liberar mapeamento de P4
    P4MAP0 = PM_UCA1TXD;   //P4.0 = TXD
    P4MAP3 = PM_UCA1RXD;   //P4.3 = RXD
    UCA1CTL1 = UCSSEL_2;   //RST=0 e Selecionar SMCLK
}

// Configurar GPIO
void gpio_config(void){;}
