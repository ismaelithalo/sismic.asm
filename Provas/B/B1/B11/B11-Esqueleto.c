// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Quest�o 1

#include <msp430.h> 

// Prot�tipo de suas fun��es
char rand_8b(void);
void rand_vet(unsigned char *vt, int qtd);
void ord_vet(unsigned char *vt, int qtd);
char media_vet(unsigned char *vt, int qtd);
char freq_nr(unsigned char nr, unsigned char *vt, int qtd);

// Fun��es b�sicas para o gerador aleat�rio de 2 bits
char rand_2b(void);
void adc_config(void);
void tb0_config(void);

int main(void){
	unsigned char vetor[256];
	volatile char max,max_freq;
	volatile char min,min_freq;
	volatile char moda,moda_freq;
	volatile char media;
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	adc_config();
	tb0_config();
	
// Seu c�digo

	return 0;
}

// Abaixo est�o as 5 fun��es que o aluno deve escrever
char rand_8b(void){;}
void rand_vet(unsigned char *vt, int qtd){;}
void ord_vet(unsigned char *vt, int qtd){;}
char media_vet(unsigned char *vt, int qtd){;}
char freq_nr(unsigned char nr, unsigned char *vt, int qtd){;}


////////////////////////////////////////////
// A seguir est�o as fun��es b�sicas      //
// N�o altere as fun��es abaixo           //
// Use o gerador aleat�rio de 2 bits      //
////////////////////////////////////////////

// Gerar um n�mero aleat�rio de 2 bits
char rand_2b(void){
    volatile unsigned int *pt;
    unsigned int x,i;
    pt=&ADC12MEM0;
    ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
    while ( (ADC12IFG&ADC12IFG15)==0);
    x=0;
    for (i=0; i<16; i++){
        x+=pt[i]&BIT0;
    }
    return x&3;
}

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
    for (i=0; i<16;  i+=2){
        pt[i+0]=ADC12SREF_0 | ADC12INCH_14; //ADC12MCTL0 at� ADC12MCTL7
        pt[i+1]=ADC12SREF_0 | ADC12INCH_15; //ADC12MCTL8 at� ADC12MCTL15
    }
    pt[15] |= ADC12EOS;                  //EOS em ADC12MCTL15
    ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
}

void tb0_config(void){
    TB0CTL = TBSSEL_2 | MC_1;
    TB0CCTL1 = OUTMOD_6;    //Out = modo 6
    TB0CCR0 = 104;          //10 kHz
    TB0CCR1 = TBCCR0/2;    //Carga 50%
}
