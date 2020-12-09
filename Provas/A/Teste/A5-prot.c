#include <msp430.h>

void atraso(long x);
void ADC_config();
void io_config();
void timer_config();

volatile int flag = 0, i=0;                                          // Variaveis
volatile unsigned long int can0=0, can1=0, media_a0=0, media_a1=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    io_config();
    ADC_config();      // Configura o ADC
    timer_config();

    while(1) {
        if(flag == 1)                                               // Se a bandeira eh 1 significa que o LCD deve ser
        {
            P1OUT ^= BIT0;                                                        // atualizado
            // updateLCD(resultX, resultY);                            // Atualiza o LCD com as medicoes obtidas
            flag = 0;                                               // Zera a flag
        }
    }

    return 0;
}


void atraso(long x){
    volatile long cont=0;
    while(cont++ < x) ;
}

void io_config() {

    // Configura a chave SW2
    P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
    P1REN |= BIT1;              //Habilitar resistor
    P1OUT |= BIT1;              //Habilitar pullup

    // Configura a chave SW6.2
    P6DIR &= ~BIT2;
    P6REN |= BIT2;
    P6OUT |= BIT2;

    // Configura os LEDS
    P1DIR |= BIT0;              //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;             //Led1 apagado

    P4DIR |= BIT7;              //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;             //Led1 apagado
}

void ADC_config() {
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

        ADC12MCTL0  =   0;
        ADC12MCTL1  =   1 | ADC12EOS;                 // End of Sequence

        ADC12IE     =   BIT1;                                // Interrup apenas no final
        ADC12CTL0  |=   ADC12ENC;                            // Habilita pois a configuracao acabou

        __enable_interrupt();                               // Habilita interrupcoes globais
        // ADC12IFG = 0;
}

void timer_config() {
    TA0CTL   = TASSEL__ACLK | MC__UP;                               // Configura um timer para enviar o trigger ao ADC12
    TA0CCR0  = 0x71C;                                               // Envia o trigger a cada 1/8 segundos
    TA0CCTL0 = CCIE;                                                // Habilita interrupcao do timer
}


#pragma vector = TIMER0_A0_VECTOR                                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                                     // Quando o CCR0 chega ao maximo, acontece a interrupcao
{
    ADC12CTL0  &=  ~ADC12SC;                                        // Envia um trigger ao ADC12
    ADC12CTL0  |=   ADC12SC;                                        // para que ele realize uma medicao
}

#pragma vector = ADC12_VECTOR                                       // Interrupcao ADC12
__interrupt void ADC_ISR()
{
    can0 += ADC12MEM0;                                              // Soma a medicao de X a variavel can0
    can1 += ADC12MEM1;                                              // Soma a medicao de Y a variavel can1
    i++;                                                            // Incrementa i

    if(i == 16)                                                      // Quando as 8 amostras de cada ja tiverem sido colhidas
    {
        media_a0 = can0/16;                                           // Calcula a media de X
        media_a1 = can1/16;                                           // Calcula a media de Y
        flag = 1;                                                   // Seta a flag para indicar isso
        can0 = 0;                                                   // Prepara essas variaveis para a proxima soma
        can1 = 0;
        i = 0;                                                      // Zera o i
    }
}

