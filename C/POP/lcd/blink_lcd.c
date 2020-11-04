#include <msp430.h>

void init_i2c();
void pcf_write(char data);
void delay();


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Configura o LED como saída
    P1SEL &= ~BIT0;
    P1DIR |=  BIT0;
    P1OUT &= ~BIT0;

    init_i2c();

    volatile int i;
    while(1)
    {
        P1OUT |= BIT0;    // Liga o LED
        pcf_write(0xFF);  // Acende o LCD
        for(i = 0; i < 2; i++) // 10 x Delay
            delay();

        P1OUT &= ~BIT0;   // Desliga o LED
        pcf_write(0x00);  // Apaga o LCD
        for(i =0; i < 2; i++) // 10 x Delay
            delay();
    }

    return 0;
}


void init_i2c()
{
    // 1. Set UCSWRST
    UCB0CTL1 |= UCSWRST;

    // 2. Initialize all USCI registers with UCSWRST = 1.
    UCB0CTL0 = UCMST     // Master mode
             + UCMODE_3  // I2C mode
             + UCSYNC;   // Synchronous mode
    UCB0CTL1 = UCSSEL__ACLK  // ACLK
             + UCTR          // Transmitter
             + UCSWRST;      // Manter o reset acionado durante a configuração do módulo.

    // Divisor de clock = 4
    UCB0BR0 = 4;
    UCB0BR1 = 0;

    // Endereço do LCD
    UCB0I2CSA = 0x27;  // PCF8574T
    //UCB0I2CSA = 0x3F;  // PCF8574FT

    // 3. Configure ports.
    P3SEL |= BIT0;  // P3.0 controlado pelo USCI
    P3REN |= BIT0;  // Habilitar resistor no pino
    P3OUT |= BIT0;  // Escolhe resistor de pull-up.

    P3SEL |= BIT1;  // P3.1 controlado pelo USCI
    P3REN |= BIT1;  // Habilitar resistor no pino
    P3OUT |= BIT1;  // Escolhe resistor de pull-up.

    // 4. Clear UCSWRST through software
    UCB0CTL1 &= ~UCSWRST;

    // 5. Enable interrupts (optional).
}


void pcf_write(char data)
{
    UCB0IE = 0;  // Desliga as interrupções

    //UCB0I2CSA = ... // Configura o endereço para comunicação

    while(UCB0STAT & UCBBUSY);  // Espera até o barramento estar liberado para comunicação

    UCB0CTL1 |= UCTXSTT;  // Pede o start

    while((UCB0IFG & UCTXIFG) == 0); // Espera o buffer ficar disponível.

    UCB0TXBUF = data;  // Transmite o dado.

    while(UCB0CTL1 & UCTXSTT); // Espera o envio do endereço e recebimento do ACK.

    if((UCB0IFG & UCTXNACK) != 0)
    { // Recebi um NACK
        UCB0CTL1 |= UCTXSTP; // Stop
    } else
    { // Recebi um ACK
        UCB0CTL1 |= UCTXSTP;
    }

    return;
}


void delay()
{
    volatile unsigned int x = 60000;
    while(x > 0)
        x--;
}
