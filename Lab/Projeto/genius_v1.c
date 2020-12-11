#include <msp430.h> 

#define TRUE    1
#define FALSE   0

#define FREQ   700

#define c 261		//DÓ
#define d 294		//Ré
#define e 329		//Mi
#define f 349		//Fá

void io_config();
void adc_config(void);
void ta0_config(void);

volatile int i, medx, medy, flag=0;
volatile unsigned int can0, can1;

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    io_config();
    adc_config();
    ta0_config();
    t2_inic();

    __enable_interrupt();

    while(TRUE){
        if (flag) {
            if (medx > 2160){
                P4OUT |= BIT7;
                P1OUT |= BIT0;
                nota(c, 300);
            }
            else if (medx < 2020){
                P4OUT |= BIT7;
                P1OUT |= BIT0;
                nota(d, 300);
            }
            else {
                P4OUT &=  ~BIT7;
                P1OUT &=  ~BIT0;
                nota(0,100);
            }

            if (medy > 2150) {
                P4OUT |= BIT7;
                P1OUT &=  ~BIT0;
                nota(e, 300);
            }
            else if (medy < 2010) {
                P4OUT &=  ~BIT7;
                P1OUT |= BIT0;
                nota(f, 300);
            } 
        }
    }

    return 0;

}

#pragma vector = ADC12_VECTOR    //54
__interrupt void adc_isr(void){

        can0 += ADC12MEM0;                      // Soma a medicao de X a variavel can0
        can1 += ADC12MEM1;                      // Soma a medicao de Y a variavel can1
        i++;                                    // Incrementa i (contador de conversões)

        if(i == 16){                            // Quando as 8 amostras de cada ja tiverem sido colhidas
                                    
                medx = can0/16;                 // Calcula a media de X
                medy = can1/16;                 // Calcula a media de Y
                flag = 1;                       // Seta a flag para indicar isso
                can0 = 0;                       // Prepara essas variaveis para a proxima soma
                can1 = 0;
                i = 0;                          // Zera o i
                // P4OUT ^= BIT7;
        }
 }

#pragma vector = TIMER0_A0_VECTOR                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                     // Quando o CCR0 chega ao maximo, acontece a interrupcao
{
    ADC12CTL0  &=  ~ADC12SC;                        // Envia um trigger ao ADC12
    ADC12CTL0  |=   ADC12SC;                        // para que ele realize uma medicao
    // P1OUT ^= BIT0;                               // Teste de frequência do timer
}

// Configurar ADC
void adc_config(void){ 
    ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo
    P6SEL |= (BIT0 + BIT1);         // Habilita as entradas em P6.0 P6.1
    ADC12CTL0 = ADC12SHT1_3 +       // Sample and hold time
                ADC12SHT0_3 +       // Sample and hold time
                ADC12ON     +       // Liga o ADC
                ADC12MSC;           // Modo sequencial!
    ADC12CTL1 = ADC12CSTARTADD_0 +  // Inicio da sequencia = canal 0.
                ADC12SHS_0 +        // Sample and holder "manual"
                ADC12SHP +          // Sampling signal = sampling timer
                ADC12DIV_0 +        // Divisor de clock.
                ADC12SSEL_0 +       // Oscilador do ADC
                ADC12CONSEQ_1;      // Sequence of channels
    ADC12CTL2 = ADC12TCOFF +        // Desliga o sensor de temperatura
                ADC12RES_2;         // Resolução = 12 bits

    ADC12IE     =   BIT1;           // Interrup apenas no final

    ADC12MCTL0  =   0;
    ADC12MCTL1  =   1 | ADC12EOS;   // End of Sequence
    
    ADC12CTL0  |=   ADC12ENC;       // Habilita pois a configuracao acabou
 }

// Configurar TA0
void ta0_config(void){ 
    TA0CTL   = TASSEL__ACLK | MC__UP;               // ACLK e modo 1
    TA0CCR0  = FREQ;                                // Ponto de disparo da interrupção
    TA0CCTL0 = CCIE;                                // Habilita interrupcao do timer

    P1DIR |= BIT2;                                  //Sair TA0.1
    P1SEL |= BIT2;
 }


void t2_inic(void){
    TA2CTL = TASSEL_2|ID_0|MC_1|TACLR;
    TA2EX0 = TAIDEX_0;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR0 = 2*1048;
    TA2CCR2 = TA2CCR0/2;
}

 // Configurar I/O
void io_config() {

    // Configura a chave SW2
    P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
    P1REN |= BIT1;              //Habilitar resistor
    P1OUT |= BIT1;              //Habilitar pullup

    // Configura a chave SW6.2
    P6DIR &= ~BIT2;
    P6REN |= BIT2;
    P6OUT |=     BIT2;

    // Configura os LEDS
    P1DIR |= BIT0;              //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;             //Led1 apagado

    P4DIR |= BIT7;              //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;             //Led1 apagado

    P2DIR |= BIT5;	//Saída PWM
    P2OUT &= ~BIT5;
    P2SEL |= BIT5;
}

// Gerar tom com frequência = "freq"
// Durante período = tempo mili_seg
void nota(int freq, int tempo){
    volatile int cont=0,comp0,comp2;
    if (freq != 0){
        comp0=TA2CCR0 = 1048576L/freq;
        TA2CCR0 = comp0;				//Prog freq

        comp2 = TA2CCR0/2;
        TA2CCR2 = comp2;				//Carga 50%
        cont = freq*(tempo/1000.);		//Quantidade de períodos
    }
    else{
        P2SEL &= ~BIT5;					//Desligar saída
        comp0=TA2CCR0 = 1048576L/1000;	//Programar 1 kHz
        comp2=TA2CCR2 = TA2CCR0/2;		//Carga  50%
        cont = tempo;					//Quantidade de períodos
    }
    while(cont != 0){
        while( (TA2CCTL0&CCIFG) == 0);	//Usar CCIFG para
        TA2CCTL0 &= ~CCIFG;				//contar quantidade
        cont--;							//de períodos
    }
    P2SEL |= BIT5;
}
