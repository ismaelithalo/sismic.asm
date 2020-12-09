// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// B2p5-Esqueleto.C
//
// Conex�es I2C
// SCL : P3.0 ===> P4.1
// SDA : P3.1 <==> P4.2

#include <msp430.h> 

#define TRUE  1
#define FALSE 0
#define SEMENTE 5

// Fun��es disponibilizadas
// N�o devem ser alteradas
void USCI_B1_config(void);
void USCI_B1_rx(void);
void USCI_B1_tx(void);

// Fun��es a serem escritas
void leds(char z);
void escr_wr(char x);
char escr_busca(void);
void USCI_B0_config(void);
void io_config(void);

int main(void){
    volatile char cont=0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    io_config();
    USCI_B0_config();
    USCI_B1_config();

    while(TRUE){
        //Coloque seu c�digo dentro deste la�o
        ;
    }
    return 0;
}

// Atualizar os leds em fun��o de z
// z = 0, 1, 2 ou 3
void leds(char z){;}

// Escreve um byte no escravo
void escr_wr(char x){;}

// Retorna o endere�o do escravo
char escr_busca(void){;}

// Configurar USCI_B0 como mestre
// P3.0 = SDA e P3.1 = SCL
void USCI_B0_config(void){;}

// Configurar GPIO
void io_config(void){;}

//////////////////////////////////////////////////////
//// As fun��es abaixo n�o devem ser alteradas ///////
//////////////////////////////////////////////////////

void USCI_B1_config(void){
    UCB1CTL1 = UCSWRST;         //Resetar USCI_B1
    UCB1CTL0 = UCMODE_3 |       //Modo I2C
               UCSYNC;          //S�ncrono
    UCB1I2COA = 5;             //Endere�o do Escravo
    UCB1CTL1 = 0;               //UCSWRST=0
    UCB1IE = UCTXIE | UCRXIE;   //Hab interrp TX e RX
    P4SEL |= BIT2 | BIT1;       //Fun��o alternativa
    P4REN |= BIT2 | BIT1;       //Hab resistor
    P4OUT |= BIT2 | BIT1;       //Pullup
    __enable_interrupt();       //GIE=1, hab int geral
}

// ISR da USCI_B1
#pragma vector = USCI_B1_VECTOR         //Vetor 45
__interrupt void isr_USCI_B1(void){
    static unsigned int nend=SEMENTE;
    int n;
    n = __even_in_range(UCB1IV,0xC);    //Consultar UCB1IV
    switch(n){
        case 0x0: break;            case 0x2: break;
        case 0x4: break;            case 0x6: break;
        case 0x8: break;
        case 0xA: USCI_B1_rx();  break; //Rotina RX
        case 0xC: USCI_B1_tx();  break; //Rotina TX
    }
    nend = (nend*487) % 127;
    if (nend==0)    nend=1;
    UCB1I2COA = nend;             //Novo Endere�o do Escravo
}

// Rotina para receber um byte e guardar no vetor
void USCI_B1_rx(void){
    char x;
    x=UCB1RXBUF;
    switch(x){
    case '1':   P1OUT ^= BIT0;  break;
    case '2':   P4OUT ^= BIT7;  break;
    case '3':   P1OUT |= BIT0;  P4OUT |= BIT7;  break;
    }
}

// Rotina para transmitir o contador e increment�-lo
void USCI_B1_tx(void){;}

