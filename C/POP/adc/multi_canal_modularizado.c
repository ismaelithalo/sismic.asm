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

    volatile unsigned int buffer_a0[8],buffer_a1[8],i,voltagem_a0_uni,voltagem_a0_dec,voltagem_a1_uni, voltagem_a1_dec;
    volatile unsigned long int media_a0=0, media_a1=0;

    while(TRUE) {

        while(P6IN&BIT2);

         //Apaga os leds
        P1OUT &= ~BIT0;
        P4OUT &= ~BIT7;

        // Gera um pulso para iniciar a conversão
        ADC12CTL0 &= ~ADC12SC;
        ADC12CTL0 |= ADC12SC;

        // Espera a conversão terminar no segundo canal!
        while(!(ADC12IFG & ADC12IFG1));

        // Guarda o dado convertido
        buffer_a0[0] = ADC12MEM0;     buffer_a1[0] = ADC12MEM1;
        buffer_a0[1] = ADC12MEM2;     buffer_a1[1] = ADC12MEM3;
        buffer_a0[2] = ADC12MEM4;     buffer_a1[2] = ADC12MEM5;
        buffer_a0[3] = ADC12MEM6;     buffer_a1[3] = ADC12MEM7;
        buffer_a0[4] = ADC12MEM8;     buffer_a1[4] = ADC12MEM9;
        buffer_a0[5] = ADC12MEM10;    buffer_a1[5] = ADC12MEM11;
        buffer_a0[6] = ADC12MEM12;    buffer_a1[6] = ADC12MEM13;
        buffer_a0[7] = ADC12MEM14;    buffer_a1[7] = ADC12MEM15;

        // Soma todos os valores das conversões
        for (i=0; i<8; i++){
            media_a0 = media_a0 + buffer_a0[i];
            media_a1 = media_a1 + buffer_a1[i];
        }

        // Faz três rotações para a direita que é o equivalente a dividir por 8
        media_a0 = (media_a0 >> 3);
        media_a1 = (media_a1 >> 3);

        // (3,3-0)/4094 = 806 micro volts  * media = conversão para volts
        voltagem_a0_uni = (media_a0*806)/1000000;
        voltagem_a0_dec = (media_a0*806)%1000000;
        voltagem_a1_uni = (media_a1*806)/1000000;
        voltagem_a1_dec = (media_a1*806)%1000000;

        if(buffer_a0[0] & BITB){
            P1OUT |= BIT0;
        }
        else{
            P4OUT |= BIT7;
        }

        atraso(60000);

        ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!
        ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!

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

        // Aproveitando os 16 canais do conversor, as 16 coletas serão feitas de uma única vez porém ALTERNADAS
        // ADC12SREF_0 - Vref- = GND; Vref+ = Vcc = 3.3V  - Referências de conversão

        ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;   // Memória 0 = canal 0.
        ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1;   // Memória 1 = canal 1.
        ADC12MCTL2 = ADC12SREF_0 + ADC12INCH_0;   // Memória 2 = canal 0.
        ADC12MCTL3 = ADC12SREF_0 + ADC12INCH_1;   // Memória 3 = canal 1.
        ADC12MCTL4 = ADC12SREF_0 + ADC12INCH_0;   // Memória 4 = canal 0.
        ADC12MCTL5 = ADC12SREF_0 + ADC12INCH_1;   // Memória 5 = canal 1.
        ADC12MCTL6 = ADC12SREF_0 + ADC12INCH_0;   // Memória 6 = canal 0.
        ADC12MCTL7 = ADC12SREF_0 + ADC12INCH_1;   // Memória 7 = canal 1.
        ADC12MCTL8 = ADC12SREF_0 + ADC12INCH_0;   // Memória 8 = canal 0.
        ADC12MCTL9 = ADC12SREF_0 + ADC12INCH_1;   // Memória 9 = canal 1.
        ADC12MCTL10 = ADC12SREF_0 + ADC12INCH_0;   // Memória 10 = canal 0.
        ADC12MCTL11 = ADC12SREF_0 + ADC12INCH_1;   // Memória 11 = canal 1.
        ADC12MCTL12 = ADC12SREF_0 + ADC12INCH_0;   // Memória 12 = canal 0.
        ADC12MCTL13 = ADC12SREF_0 + ADC12INCH_1;   // Memória 13 = canal 1.
        ADC12MCTL14 = ADC12SREF_0 + ADC12INCH_0;   // Memória 14 = canal 0.
        ADC12MCTL15 = ADC12SREF_0 +                // Memória 15 = canal 1.
                      ADC12INCH_1 +
                      ADC12EOS;                    // Fim da sequência de conversão    
                    
        ADC12IFG = 0;

        ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!
}
