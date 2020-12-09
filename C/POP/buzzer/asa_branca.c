// Tocar a Marcha Imperial
// Star Wars

// Usar um buzzer passivo (sem oscilador interno)
// Cuidado: a maioria dos buzzers à venda tem oscilado interno
// Sugestões:
// https://www.filipeflop.com/produto/modulo-buzzer-5v-passivo/
// https://produto.mercadolivre.com.br/MLB-1014846215-buzzer-passivo-5v-arduino-pic-_JM

// Ligar o "+" no pino P2.5
// Ligar o "-" no GND

// Inspirado em:
//https://dragaosemchama.com/2014/04/player-de-marcha-imperial-8-bitsarduino/

//for the sheet music see:
//http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0016254
//this is just a translation of said sheet music to frequencies / time in ms
//used 500 ms for a quart note
	
#include <msp430.h> 

// Notas
#define c 261		//DÓ
#define d 294		//Ré
#define e 329		//Mi
#define f 349		//Fá
#define g 391		//Sol
#define gS 415	
#define a 440
#define aS 455
#define b 466
#define cH 523
#define cSH 554
#define dH 587
#define dSH 622
#define eH 659
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880

void pin_inic(void);
void t2_inic(void);
void nota(int freq, int tempo);
void leds(int dado);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    pin_inic();
    t2_inic();

    /*
    while(1){
        P2OUT |= BIT4;
        nota(a, 500);
        P2OUT &= ~BIT4;
        nota(0,100);        //
    }
    */

    while(1){

        leds(0);            //00

        nota(c, 300);
        nota(d, 300);
        nota(e, 300);
        nota(0,100);
        nota(g, 300);
        nota(0,50);
        nota(g, 300);
        nota(0,100);
        nota(e, 300);
        nota(0,100);
        nota(f, 300);
        nota(0,50);
        nota(f, 300);
        nota(0,100);
        nota(0,100);
        nota(c, 300);
        nota(d, 300);
        nota(e, 300);
        nota(0,100);
        nota(g, 300);
        nota(0,50);
        nota(g, 300);
        nota(f, 300);
        nota(0,50);
        nota(e, 300);
        nota(0,100);
        nota(0,100);
        nota(c, 300);
        nota(0,50);
        nota(c, 300);
        nota(d, 300);
        nota(e, 300);
        nota(0,50);
        nota(g, 300);
        nota(0,150);
        nota(g, 300);
        nota(f, 300);
        nota(e, 300);
        nota(c, 300);
        nota(0,50);
        nota(f, 400);
        nota(0,120);
        nota(f, 300);
        nota(0,50);
        nota(e, 300);
        nota(0,100);
        nota(d, 300);
        nota(0,50);
        nota(d, 300);
        nota(0,50);
        nota(e, 300);
        nota(0,100);
        nota(d, 300);
        nota(0,50);
        nota(d, 300);
        nota(0,50);
        nota(c, 300);
        nota(0, 50);
        nota(c, 300);
        nota(0,1000);
                //Pausa
        

    }
    return 0;
}

void leds(int dado){
    switch(dado){
        case 0: P1OUT &= ~BIT0;  P4OUT &= ~BIT7;     break;
        case 1: P1OUT &= ~BIT0;  P4OUT |=  BIT7;     break;
        case 2: P1OUT |=  BIT0;  P4OUT &= ~BIT7;     break;
        case 3: P1OUT |=  BIT0;  P4OUT |=  BIT7;     break;
    }
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


void pin_inic(void){
    P2DIR |= BIT5;	//Saída PWM
    P2OUT &= ~BIT5;
    P2SEL |= BIT5;

    P2DIR |= BIT4;	//Não usado?
    P2OUT &= ~BIT4;

    P1DIR |= BIT0;	//LED VM
    P1OUT &= ~BIT0;

    P4DIR |= BIT7;	//LED VD
    P4OUT &= ~BIT7;
}

void t2_inic(void){
    TA2CTL = TASSEL_2|ID_0|MC_1|TACLR;
    TA2EX0 = TAIDEX_0;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR0 = 2*1048;
    TA2CCR2 = TA2CCR0/2;
}

