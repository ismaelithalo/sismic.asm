#include <msp430.h> 

void delay()
{
    volatile unsigned int x = 60000;
    while(x > 0)
        x--;
}

void initADC()
{
    ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!

    P6SEL |= (BIT0 + BIT1);     // Habilita as entradas em P6.0 P6.1

    ADC12CTL0 = ADC12SHT1_3 +       // Sample and hold time
                ADC12SHT0_3 +       // Sample and hold time
                ADC12ON     +       // Liga o ADC
                ADC12MSC;       // Modo sequencial!
    ADC12CTL1 = ADC12CSTARTADD_0 +  // Inicio da sequencia = canal 0.
                ADC12SHS_0 +        // Sample and holder "manual"
                ADC12SHP +          // Sampling signal = sampling timer
                ADC12DIV_0 +        // Divisor de clock.
                ADC12SSEL_0 +       // Oscilador do ADC
                ADC12CONSEQ_1;   // Sequence of channels
    ADC12CTL2 = ADC12TCOFF +        // Desliga o sensor de temperatura
                ADC12RES_2;         // Resolução = 12 bits

    // Canal 0
    ADC12MCTL0 = ADC12SREF_0 +       // Vref- = GND; Vref+ = Vcc = 3.3V
                 ADC12INCH_0;       // Memória 0 = canal 0.
    // Canal 1
    ADC12MCTL1 = ADC12SREF_0 +       // Vref- = GND; Vref+ = Vcc = 3.3V
                 ADC12INCH_1 +      // Memória 1 = canal 1.
                 ADC12EOS;          // Fim da sequencia no canal 1

    ADC12IFG = 0;

    ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Configura a chave SW2
    P1SEL &= ~BIT1;
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    // Configura os LEDS
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    P4DIR |= BIT7;
    P4DIR &= ~BIT7;

    volatile unsigned int x = 0;
    volatile unsigned int y = 0;
    initADC();      // Configura o ADC

    while(1)
    {
        while(P1IN & BIT1);

        //Apaga os leds
        P1OUT &= ~BIT0;
        P4OUT &= ~BIT7;

        // Gera um pulso para iniciar a conversão
        ADC12CTL0 &= ~ADC12SC;
        ADC12CTL0 |= ADC12SC;

        // Espera a conversão terminar no segundo canal!
        while(!(ADC12IFG & ADC12IFG1));

        // Guarda o dado convertido
        x = ADC12MEM0;
        y = ADC12MEM1;

        if(x & BITB)
        {
            P1OUT |= BIT0;
        }
        else
        {
            P4OUT |= BIT7;
        }

        delay();


        ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!
        ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!

    }

    return 0;
}
