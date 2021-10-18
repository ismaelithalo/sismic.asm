#include <msp430.h>

#define TRUE 1
#define FALSE 0

void USCI_A1_config(void);

volatile unsigned char vet[100];

int main(void){
    
    char x;
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    
    USCI_A1_config();

    bt_str("Teste 1 2 3\n"); //Boas vindas

    while(TRUE){ //Laço principal
        if ( (UCA1IFG&UCTXIFG) == UCTXIFG){ //TX0 ocioso?

               P1OUT ^= BIT0;
        }
    }
    return 0;
} 

// Funcao que recebe um array de char e manda para funcao de escrita
void bt_str(char *vet){
    
    unsigned int i=0;
    while (vet[i] != '\0'){
        bt_char(vet[i++]);
    }
     
}

// Funcao que recebe um char e escreve no barramamento UART
void bt_char(char c) {

    while ( (UCA1IFG&UCTXIFG)==0); //Esperar TXIFG=1
    UCA1TXBUF=c;     
    
}

// Configurar USCI_A1 em 38.400
void USCI_A1_config(void){
    
    UCA1CTL1 = UCSWRST; //RST=1
    UCA1CTL0 = 0; //sem paridade, 8 bits, 1 stop, modo UART
    UCA1BRW = 27; // Divisor
    UCA1MCTL = UCBRS_2 | UCBRF_0; //Moduladores 2, UCOS16=0
    UCA1CTL1 = UCSSEL_2; //RST=0 e Selecionar SMCLK
    
    P4SEL |= BIT4 | BIT5; //Disponibilizar para a serial conversar com PC
    
    // UCA1IE = UCRXIE; //Interrupção por recepção
    
} 
