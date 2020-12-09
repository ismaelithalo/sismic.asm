// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// B2p3-Esqueleto.C
//
// Conex�es LCD
// P3.0 = SDA
// P3.1 = SCL

#include <msp430.h> 

#define TRUE    1
#define FALSE   0
#define QTD     512     //Quantidade de convers�es (amostras) do ADC

#define ADC12MEM0_ADR (__SFR_FARPTR) 0x720  //Endere�o da mem�ria 0 do ADC

// Fun��es que dever�o ser escritas
void ta0_config(void);
void dma_config(void);
void adc_config(void);
void adc_canal(char canal);

volatile int vetor[QTD];    //Vetor para receber as aquisi��es

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Suas inicializa��es
    ta0_config();
    dma_config();
    adc_config();

    while(TRUE){
        //Coloque seu c�digo dentro deste la�o
        ;
    }

    return 0;
}

// Configurar TA0.1 para PWM
// Freq = 50.000 Hz e Ciclo de Carga = 50%
void ta0_config(void){
    ;
    P1DIR |= BIT2;      //PWM saindo por P1.2
    P1SEL |= BIT2;      //Permite conferir freq do PWM
}

// Fun��o para configurar o ADC
void adc_config(void){ ; }

// Fun��o para trocar o canal do ADC
void adc_canal(char canal){ ; }

// Fun��o para configurar o DMA
void dma_config(void){  ;  }

//////////////////////////////////////////////////////
//////// Daqui em  diante n�o ser� corrigido /////////
//////// Coloque abaixo suas demais fun��es  /////////
//////////////////////////////////////////////////////

