// 190030194 - Ismael ithalo  Barbosa Pinto
// Gabarito prova A3
#include <msp430.h>

// Protótipo de suas funções
char ler_escravo(char adr);
void USCI_B0_config(void);
void io_config(void);

// Funções originais
void USCI_B1_config(void);
void USCI_B1_rx(void);
void USCI_B1_tx(void);

#define CLK50 20// SMCLK = 1.048.576, dividido por 50000, da 20,9

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    USCI_B1_config();
    USCI_B0_config();
    io_config();

    while(1){
        char cod_sw = ler_escravo(37);

        if (cod_sw == 0){ P1OUT &= ~BIT0; P4OUT &= ~BIT7;}
        else if (cod_sw == 1){ P1OUT &= ~BIT0; P4OUT |= BIT7;}
        else if (cod_sw == 2){ P1OUT |= BIT0; P4OUT &= ~BIT7;}
        else if (cod_sw == 3){ P1OUT |= BIT0; P4OUT |= BIT7;}
    }

    return 0;
}

// Funções a serem escritas
char ler_escravo(char adr){
    UCB0I2CSA = adr;
    UCB0CTL1 |= UCTXSTT;                        //Gerar START
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);   //Esperar TXSTT=0 (START)
    UCB0CTL1 |= UCTXSTP;                        //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);   //Esperar STOP

    while ( (UCB0IFG & UCRXIFG) == 0);          //Esperar RXIFG=1

    return UCB0RXBUF;                           //Retornar o dado
}


// Configurar os pinos necessarios
void io_config(void){

        P1DIR |= BIT0;              //Led1 = P1.0 = saída
        P1OUT &= ~BIT0;             //Led1 apagado

        P4DIR |= BIT7;              //Led2 = P4.7 = saída
        P4OUT &= ~BIT7;             //Led1 apagado

        P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
        P2REN |= BIT1;              //Habilitar resistor
        P2OUT |= BIT1;              //Habilitar pullup

        P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
        P1REN |= BIT1;              //Habilitar resistor
        P1OUT |= BIT1;              //Habilitar pullup

}

void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;                     //Ressetar USCI_B0
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;   //Modo mestre | Modo I2C | Síncrono
    UCB0BRW = CLK50;
    UCB0CTL1 = UCSSEL_3;                    //SMCLK e UCSWRST=0

    P3SEL |= BIT0 | BIT1;                   //Função alternativa
    P3REN |= BIT0 | BIT1;                   //Hab resistor
    P3OUT |= BIT0 | BIT1;                   //Pullup

}


/////////////////////////////////////////////////////
//////////////// Funções do ER 9.1 //////////////////
/////////////////////////////////////////////////////

// Esta função precisa ser alterada
void USCI_B1_tx(void){

    if ((P2IN&BIT1) == 0){
        if ( (P1IN&BIT1) == 0)
            UCB1TXBUF = 3;
        else
            UCB1TXBUF = 2;
    }
    else {
        if ( (P1IN&BIT1) == 0)
            UCB1TXBUF = 1;
        else
            UCB1TXBUF = 0;
    }
}

// Não alterar esta função
void USCI_B1_config(void){
    UCB1CTL1 = UCSWRST;
    UCB1CTL0 = UCMODE_3 | UCSYNC;
    UCB1I2COA = 37;
    UCB1CTL1 = 0;
    UCB1IE = UCTXIE | UCRXIE;
    P4SEL |= BIT2 | BIT1;
    P4REN |= BIT2 | BIT1;
    P4OUT |= BIT2 | BIT1;
    __enable_interrupt();
}

// Não alterar esta função
#pragma vector = USCI_B1_VECTOR         //Vetor 45
__interrupt void isr_USCI_B1(void){
    int n;
    n = __even_in_range(UCB1IV,0xC);    //Consultar UCB1IV
    switch(n){
        case 0x0: break;            case 0x2: break;
        case 0x4: break;            case 0x6: break;
        case 0x8: break;
        case 0xA: USCI_B1_rx();  break; //Rotina RX
        case 0xC: USCI_B1_tx();  break; //Rotina TX
    }
}

// Não alterar esta função
void USCI_B1_rx(void){ ; }
