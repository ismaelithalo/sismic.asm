// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Quest�o 5

#include <msp430.h> 

// Prot�tipo das fun��es b�sicas
void ta2_config(void);
int rand10(void);
void adc_config(void);
void tb0_config(void);

volatile float freq,carga;	//A serem calculados

int main(void){
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    adc_config();
    tb0_config();
	ta2_config();

	// Aqui entra seu programa principal
	
	return 0;
}


////////////////////////////////////////////
// A seguir est�o as fun��es b�sicas      //
// N�o altere as fun��es abaixo           //
// 1) Gerador aleat�rio de 10 bits        //
// 2) Configura��o para TA2 gerar PWM     //
////////////////////////////////////////////

// Configurar TA2 para gerar PWM
void ta2_config(void){
    TA2CTL = TASSEL_1 | MC_1;
    TA2CCTL1 = OUTMOD_6 | OUT;
    TA2CCR0=0;
    TA2CCR1=0;
    P2DIR |= BIT4;
    P2SEL |= BIT4;
}

// Gerador de n�meros aleat�rios de 10 bits
int rand10(void){
    volatile unsigned int *pt;
    unsigned int x,i;
    pt=&ADC12MEM0;
    ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
    while ( (ADC12IFG&ADC12IFG15)==0);
    x=0;
    for (i=0; i<16; i++){
        x=x<<1;
        x+=pt[i]&BIT0;
    }
    return x>>6;
}

// Configura��o do ADC para gerar bits aleat�rios
void adc_config(void){
    volatile unsigned char *pt;
    unsigned char i;
    ADC12CTL0  &= ~ADC12ENC;         //Desabilitar para configurar
    ADC12CTL0 = ADC12ON;             //Ligar ADC
    ADC12CTL1 = ADC12CONSEQ_1    |   //Modo sequ�ncia de canais
                ADC12SHS_3       |   //TB0.1
                ADC12CSTARTADD_0 |   //Resultado a partir de ADC12MEM0
                ADC12SSEL_3      |   //ADC12CLK = SMCLK
                ADC12REFON;

    pt=&ADC12MCTL0;
    for (i=0; i<16;  i++)
        pt[i]=ADC12SREF_0 | ADC12INCH_14; //ADC12MCTL0 at� ADC12MCTL7
    for (i=8; i<16; i++)
        pt[i]=ADC12SREF_0 | ADC12INCH_15; //ADC12MCTL8 at� ADC12MCTL15
    pt[15] |= ADC12EOS;                  //EOS em ADC12MCTL15

    ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
}

// Configura��o de TB0 para disparar as convers�es
void tb0_config(void){
    TB0CTL = TBSSEL_2 | MC_1;
    TB0CCTL1 = OUTMOD_6;    //Out = modo 6
    TB0CCR0 = 104;          //10 kHz
    TB0CCR1 = TBCCR0/2;    //Carga 50%
}
