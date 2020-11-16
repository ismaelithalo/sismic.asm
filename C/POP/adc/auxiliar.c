#include <msp430.h> 

#define TRUE 1
#define FALSE 0
#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000

void debounce(int valor);
int mon_s3(void);

void atraso(long x);
void ADC_config();
void io_config();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    io_config();
    ADC_config();      // Configura o ADC

    volatile unsigned int x = 0;
    volatile unsigned int y = 0;


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

        atraso(60000);


        ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!
        ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!

    }
    while(TRUE) {
             if (mon_s3() == TRUE) {     //Chave 1?
                 debounce(DBC);
                 P4OUT ^= BIT7;
             }
             }

    return 0;
}

void debounce(int valor){
	         volatile int x; //volatile evita optimizador do compilador
	             for (x=0; x<valor; x++); //Apenas gasta tempo
	         }

int mon_s3(void){
                 static int ps1=ABERTA; //Guardar passado de S1
                        if ( (P6IN&BIT2) == 0){ //Qual estado atual de S1?
                            if (ps1==ABERTA){ //Qual o passado de S1?
                             debounce(DBC);
                             ps1=FECHADA;
                             return TRUE;
          }
          }
                        else    {
                            if (ps1==FECHADA){            //Qual o passado de S1?
                             debounce(DBC);
                             ps1=ABERTA;
                             return FALSE;
                                        }
                             }
          return FALSE;
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
