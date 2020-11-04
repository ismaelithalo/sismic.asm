// ER8.2a USCI_A0 - Correto
//
// Comunicação serial com loop back (UCLISTEN = 1)
// Usar interrupção
// USCI_A0 Transmite e recebe letras de A até Z
// 9.600 bauds (ACLK), 8 bits, sem paridade, 1 stop

#include <msp430.h>
#define TRUE 1
#define FALSE 0

void USCI_A0_config(void);
void led_VM(void);
void led_vm(void);
void led_VD(void);
void led_vd(void);
void leds_config(void);

volatile char letra_tx,letra_rx; 

int main(void){ 

        WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer 
        leds_config();
        USCI_A0_config();
        __enable_interrupt();  

        UCA0TXBUF='A'; //Primeira transmissão

        while(TRUE){ //Laço principal
            if (letra_tx == letra_rx){ //Verificar
                led_VD(); led_vm(); //OK 
            }
            else{
                led_vd(); led_VM(); //Erro
            }
        }

        return 0;
} 

// Interrupção da USCI_A0
//#pragma vector = 56
#pragma vector = USCI_A0_VECTOR
__interrupt void usci_a0_int(void){
    volatile static letra='A';

    UCA0IV; //Apagar RXIFG
    letra_rx=UCA0RXBUF; //Letra recebida
    letra_tx=letra; //Letra transmitida

    if (++letra=='Z'+1) //Próxima letra
        letra='A'; //Voltar para letra A

    UCA0TXBUF=letra; //Transmitir
} 

// Configurar USCI_A0
void USCI_A0_config(void){
    UCA0CTL1 = UCSWRST; //RST=1 para USCI_A0
    UCA0CTL0 = 0; //sem paridade, 8 bits, 1 stop, modo UART
    UCA0STAT = UCLISTEN; //Loop Back
    UCA0BRW = 3; // Divisor
    UCA0MCTL = UCBRS_3; //Modulador = 3 e UCOS=0
    UCA0CTL1 = UCSSEL_1; //RST=0 e Selecionar ACLK
    UCA0IE = UCRXIE; //Hab. Interrup recepção
}

// Controle dos leds
void led_VM(void){ P1OUT |= BIT0; } //led Vermelho aceso
void led_vm(void){ P1OUT &= ~BIT0; } //led Vermelho apagado
void led_VD(void){ P4OUT |= BIT7; } //led Verde aceso
void led_vd(void){ P4OUT &= ~BIT7; } //led Verde apagado 

// Configurar Leds
void leds_config(void){
 P1DIR |= BIT0; P1OUT &= ~BIT0; //Led vermelho
 P4DIR |= BIT7; P4OUT &= ~BIT7; //Led verde
} 