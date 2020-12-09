// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// USCI_A0   P3.3 (TXD) ===> P4.3 (RXD) USCI_A1
//           P3.4 (RXD) <=== P4.0 (TXD)

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

void USCI_A0_config(void);
void USCI_A1_config(void);
void gpio_config(void);

void USCI_A1_tx(void);
void USCI_A1_rx(void);
char USCI_A0_2_bytes(char b1, char b2);
void USCI_A0_tx_2_bytes_led(char b1, char b2);

int flag=0;       //Flag que sinaliza pra função que lê o buffer de recepção se é o segundo byte que está sendo recebido

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    gpio_config();
    USCI_A0_config();
    USCI_A1_config();

    signed char status_s1,status_s2,cont=0;

    while(TRUE){
        
        status_s1 = USCI_A0_tx_2_bytes('S','1'); // Pergunta o estado da chave 1
        status_s2 = USCI_A0_tx_2_bytes('S','2'); // Pergunta o estado da chave 2

        if (status_s1=='F'){                    // Se a resposta da chave 1 for fechada
            cont++;                             // Incrementa o contador
            switch (cont){                      // Verifica o valor atual do contador e liga os leds da respectiva ordem
            case 1:
                USCI_A0_tx_2_bytes_led('V','D');
                USCI_A0_tx_2_bytes_led('v','m');
                break;
            case 2:
                USCI_A0_tx_2_bytes_led('V','M');
                USCI_A0_tx_2_bytes_led('v','d');
                break;
            case 3:
                USCI_A0_tx_2_bytes_led('V','M');
                USCI_A0_tx_2_bytes_led('V','D');
                break;
            case 4:                             // O valor 4 no contador indica que o passado é três, ou seja o novo valor é zero
                USCI_A0_tx_2_bytes_led('v','m');
                USCI_A0_tx_2_bytes_led('v','d');
                cont=0;
                break;
            default:
                break;
            }
        }
        if (status_s2=='F'){                    // Se a resposta da chave 2 for fechada
            cont--;                             // Decrementa o contador
            switch (cont){                      // Verifica o valor atual do contador e liga os leds da respectiva ordem
            case 1:
                USCI_A0_tx_2_bytes_led('V','D');
                USCI_A0_tx_2_bytes_led('v','m');
                break;
            case 2:
                USCI_A0_tx_2_bytes_led('V','M');
                USCI_A0_tx_2_bytes_led('v','d');
                break;
            case 3:
                USCI_A0_tx_2_bytes_led('V','M');
                USCI_A0_tx_2_bytes_led('V','D');
                break;
            case 0:
                USCI_A0_tx_2_bytes_led('v','m');
                USCI_A0_tx_2_bytes_led('v','d');
                break;
            case -1:                            // O valor -1 no contador indica que o passado é zero, ou seja o novo valor é três
                USCI_A0_tx_2_bytes_led('V','M');
                USCI_A0_tx_2_bytes_led('V','D');
                cont=3;
                break;
            default:
                break;
            }
        }

    }
    return 0;
}

// Rotina de interrupção baseada na prova A4
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

// Configurar USCI_A0 para 19.200
/*  BITCLK16 = 16 x BITCLK = 16 x 19.200 = 307.200 Hz.
    n = 1048576/307.200 = 3,41   N = 3       UCBRx = 3
    M16 = (0.41 x 16) = 6,56    M16 = 6     UCBRFx = 6
    n = 310689,18/19200 = 16,18
    M8 = (0,18 x 8) = 1,44      M8 = 1      UCBRSx = 1.
    BR = 310689,18/16+1/8 = 19.267 valor proximo de 19.200

    Cálculo de br de 19200 retirado do ER 8.4
*/
void USCI_A0_config(void){
    UCA0CTL1 = UCSWRST;         //RST=1 para USCI_A0
    UCA0CTL0 = 0;               //sem paridade, 8 bits, 1 stop, modo UART

    UCA0BRW = 3;                // Divisor
    UCA0MCTL =  UCBRF_6 | 
                UCBRS_1 | 
                UCOS16;         //Modo Super Amostragem (UCOS=1)

    P3SEL |= BIT4 | BIT3;       //Disponibilizar P3.3 e P3.4
    UCA0CTL1 = UCSSEL_2;        //RST=0 e Selecionar SMCLK
}

// Configurar USCI_A1 para 19.200
void USCI_A1_config(void){
    UCA1CTL1 = UCSWRST;
    UCA1CTL0 = 0;

    UCA1BRW = 3;
    UCA1MCTL = UCBRF_6 |   //
               UCBRS_1 |   //
               UCOS16;     //Modo Super Amostragem (UCOS=1)

    P4SEL |= BIT3 | BIT0;  //Disponibilizar P4.3 e P4.0

    PMAPKEYID = 0X02D52;   //Liberar mapeamento de P4
    P4MAP0 = PM_UCA1TXD;   //P4.0 = TXD
    P4MAP3 = PM_UCA1RXD;   //P4.3 = RXD
    UCA1CTL1 = UCSSEL_2;   //RST=0 e Selecionar SMCLK

    UCA1IE = UCRXIE;        //Liga a interrpção do escravo
     __enable_interrupt();  //GIE=1, hab int geral
}

// Configurar GPIO
void gpio_config(void){
    // Configura a chave SW2
    P1DIR &= ~BIT1;             //S2 = P1.1 = entrada
    P1REN |= BIT1;              //Habilitar resistor
    P1OUT |= BIT1;              //Habilitar pullup

    // Configura a chave SW1
    P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
    P2REN |= BIT1;              //Habilitar resistor
    P2OUT |= BIT1;              //Habilitar pullup

    // Configura os LEDS
    P1DIR |= BIT0;              //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;             //Led1 apagado

    P4DIR |= BIT7;              //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;             //Led1 apagado
}

//////////////////////////////////////////////////////
/////// Funções adicionadas para complementar ////////
/////////////////////////////////////////////////////

// Função chamada pela interrupção de recepção, ela chama a função de transmissão que lê os buffers de recepção
void USCI_A1_rx(void){
    USCI_A1_tx();
}    

/* Função de transmissão do estado das chaves e ativação dos leds
 **Se aproveitando do fato de entre as instruções do mestre para o escravo, o segundo byte não se repete nenhuma vez, a prioridade
 é saber se eu estou recebendo o segundo byte e qual é ele, por isso a flag */
void USCI_A1_tx(void){
    if (flag == 0){                         // Verifica se a flag de byte 2 está desligada
        switch (UCA1RXBUF){                 // Compara o valor do buffer de recepção
        case 'S': flag=1; break;            // Em qualquer caso, apenas seta a flag de byte 2 para 1
        case 'V': flag=1; break;
        case 'v': flag=1; break;
        default: break;
        }
    }
    else {                                  // Verifica se a flag de byte 2 está ligada
        switch (UCA1RXBUF){                 // Compara o valor do buffer de recepção
        case '1':                           // Como a flag está ligada, e o segundo byte é o valor 1, está sendo perguntado o estado da chave 1
            if (mon_s1() == TRUE)
                UCA1TXBUF = 'F';            // Retorna F caso fechada
            else
                UCA1TXBUF = 'A';            // Retorna A caso aberta
        break;
        case '2':                           // Como a flag está ligada, e o segundo byte é o valor 2, está sendo perguntado o estado da chave 2
            if (mon_s2() == TRUE)
                UCA1TXBUF = 'F';            // Retorna F caso fechada
            else
                UCA1TXBUF = 'A';            // Retorna A caso aberta
        break;
        case 'M': P1OUT |= BIT0; break;     // Como a flag está ligada, e o segundo byte é M, o mestre está mandando ligar o led 1
        case 'm': P1OUT &= ~BIT0; break;    // Como a flag está ligada, e o segundo byte é m, o mestre está mandando desligar o led 1
        case 'D': P4OUT |= BIT7; break;     // Como a flag está ligada, e o segundo byte é D, o mestre está mandando ligar o led 2
        case 'd': P4OUT &= ~BIT7; break;    // Como a flag está ligada, e o segundo byte é d, o mestre está mandando desligar o led 2
        default: break;
        }
    }  
}

// Função que manda dois bytes por pulling, um de cada vez e retorna o valor do buffer de recepção no final
char USCI_A0_tx_2_bytes(char b1, char b2) {
    volatile signed char response;

    while ( (UCA0IFG&UCTXIFG)==0);      //Esperar TXIFG=1
    UCA0TXBUF=b1;                      //Transmitir
    while ( (UCA0IFG&UCTXIFG)==0);      //Esperar TXIFG=1
    UCA0TXBUF=b2;                      //Transmitir
    while ( (UCA0IFG&UCRXIFG)==0);      //Esperar RXIFG=1
    response = UCA0RXBUF;               //Receber

    return response;
}

// Função que manda dois bytes por pulling, um de cada vez e não retorna nada, serve para os leds
void USCI_A0_tx_2_bytes_led(char b1, char b2) {
    while ( (UCA0IFG&UCTXIFG)==0);      //Esperar TXIFG=1
    UCA0TXBUF=b1;                      //Transmitir
    while ( (UCA0IFG&UCTXIFG)==0);      //Esperar TXIFG=1
    UCA0TXBUF=b2;                      //Transmitir
}

// Monitorar S1, retorna TRUE se foi acionada
int mon_s1(void){
    static int ps1=ABERTA;      //Guardar passado de S1
    if ( (P2IN&BIT1) == 0){     //Qual estado atual de S1?
        if (ps1==ABERTA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps1==FECHADA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

// Monitorar S2, retorna TRUE se foi acionada
int mon_s2(void){
    static int ps2=ABERTA;      //Guardar passado de S2
    if ( (P1IN&BIT1) == 0){     //Qual estado atual de S2?
        if (ps2==ABERTA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps2==FECHADA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

// Debounce
void debounce(int valor){
    volatile int x;
    for (x=0; x<valor; x++);
}