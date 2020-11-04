// 190030194 - Ismael Ithalo Barbosa Pinto
// A4 - Soluçao
//
// USCI_A0   P3.3 (TXD) ===> P4.3 (RXD) USCI_A1
//           P3.4 (RXD) <=== P4.0 (TXD)
// USCI_A1 retorna o estado das chaves

#include <msp430.h>

#define TRUE  1
#define FALSE 0

/*  BITCLK16 = 16 x BITCLK = 16 x 19.194 = 307.104 Hz.
    n = 1048576/307104 = 3,41   N = 3       UCBRx = 3
    M16 = (0.41 x 16) = 6,56    M16 = 7     UCBRFx = 7
    n = 305040/19194 = 15,89
    M8 = (0,89 x 8) = 7,12      M8 = 7      UCBRSx = 7.
    BR = 305040/15+7/8 = 19215 valor proximo de 19194,11
    
*/

// Valores a serem programados.
#define BR    19194 
#define UCBRW 3     //Divisor       <== Alterar
#define UCBRF 7     //Mod. 16 bits  <== Alterar
#define UCBRS 7     //Mod.  8 bits  <== Alterar
// BR programado = xx.xxx,xx bauds  <== Alterar
// Use 2 casas depois da v�rgula

void USCI_A1_tx(void);
void USCI_A1_rx(void);
void USCI_A0_config(void);
void USCI_A1_config(void);
void io_config(void);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    io_config();
    USCI_A0_config();
    USCI_A1_config();

    signed char tx_s1='1',tx_s2='2',rx_s1,rx_s2;
    
    while(TRUE){
        while ( (UCA0IFG&UCTXIFG)==0);  //Esperar TXIFG=1
        UCA0TXBUF=tx_s1;                   //Transmitir
        while ( (UCA0IFG&UCRXIFG)==0);  //Esperar RXIFG=1
        rx_s1=UCA0RXBUF;                   //Receber
        while ( (UCA0IFG&UCTXIFG)==0);  //Esperar TXIFG=1
        UCA0TXBUF=tx_s2;                   //Transmitir
        while ( (UCA0IFG&UCRXIFG)==0);  //Esperar RXIFG=1
        rx_s2=UCA0RXBUF;                   //Receber
        
        //Compara os valores e acende os leds
        if (rx_s1 == 'A' && rx_s2 == 'A'){ P1OUT &= ~BIT0; P4OUT &= ~BIT7;}
        else if (rx_s1 == 'A' && rx_s2 == 'F'){ P1OUT &= ~BIT0; P4OUT |= BIT7;}
        else if (rx_s1 == 'F' && rx_s2 == 'A'){ P1OUT |= BIT0; P4OUT &= ~BIT7;}
        else if (rx_s1 == 'F' && rx_s2 == 'F'){ P1OUT |= BIT0; P4OUT |= BIT7;}
    }
    
    return 0;
}

void USCI_A1_rx(void){
    USCI_A1_tx();
}    

void USCI_A1_tx(void){ //Monitora as chaves cconforme o valor do buffer

    if (UCA1RXBUF == '1') {
        if ( (P1IN&BIT1) == 0)
            UCA1TXBUF = 'F';
        else 
            UCA1TXBUF = 'A';
    }
    else if (UCA1RXBUF == '2') {
        if ( P2IN&BIT1 == 0)
            UCA1TXBUF = 'F';
        else
            UCA1TXBUF = 'A';
    }
    else 
        UCA1TXBUF = '?';
}    

// Configurar USCI_A0 (Mestre) - Sem interrup��es

void USCI_A0_config(void){
    UCA0CTL1 = UCSWRST;         //RST=1 para USCI_A0
    UCA0CTL0 = 0;               //sem paridade, 8 bits, 1 stop, modo UART
    UCA0BRW = 3;                // Divisor
    UCA0MCTL = UCBRF_6 | UCBRS_1 | UCOS16;  //Modulador = 3 e UCOS=0
    P3SEL |= BIT4 | BIT3;       //Disponibilizar P3.3 e P3.4
    UCA0CTL1 = UCSSEL_2;        //RST=0 e Selecionar SMCLK
}    

// ISR da USCI_A1 (Escravo)
#pragma vector = USCI_A1_VECTOR         //Vetor 46
__interrupt void isr_USCI_A1(void){
    int n;
    n = __even_in_range(UCA1IV,0xC);    //Consultar UCA1IV
    switch(n){
        case 0x0: break;
        case 0x2: USCI_A1_rx();  break; //Rotina RX
        case 0x4: USCI_A1_tx();  break; //Rotina TX
    }
}

// Configurar USCI_A1 (Escravo) - Com interrup��es
void USCI_A1_config(void){
    UCA1CTL1 = UCSWRST;
    UCA1CTL0 = 0;
    UCA1BRW = 5;           // <== Alterar
    UCA1MCTL = UCBRF_0 |   // <== Alterar
               UCBRS_0 |   // <== Alterar
               UCOS16;     //Modo Super Amostragem
    P4SEL |= BIT3 | BIT0;  //Disponibilizar P4.3 e P4.0
    PMAPKEYID = 0X02D52;   //Liberar mapeamento de P4
    P4MAP0 = PM_UCA1TXD;   //P4.0 = TXD
    P4MAP3 = PM_UCA1RXD;   //P4.3 = RXD
    UCA1CTL1 = UCSSEL_2;   //RST=0 e Selecionar SMCLK
    UCA1IE = UCRXIE;            //<== Alterar
    __enable_interrupt();  //GIE=1, hab int geral
}

// Configurar GPIO
void io_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = sa�da
    P1OUT &= ~BIT0;     //Led1 apagado

    P4DIR |= BIT7;      //Led2 = P4.7 = sa�da
    P4OUT &= ~BIT7;     //Led1 apagado

    P2DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P2REN |= BIT1;      //Habilitar resistor
    P2OUT |= BIT1;      //Habilitar pullup

    P1DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P1REN |= BIT1;      //Habilitar resistor
    P1OUT |= BIT1;      //Habilitar pullup
}
