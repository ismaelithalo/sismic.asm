// 160128200 - Jorge Roberto Silveira Filho
// Prova B1, Questão 3

#include <msp430.h>

#define FREQ_PWM 1601       //4 primeiros digitos da matrícula
#define T_PWM 654           // período
#define PASSO_5 130   //Passo de 20% no ciclo de carga
#define PASSO_7 93    // Passo de 1/7 no ciclo de carga

#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce


// Pelo menos essas 3 funções precisam estar presentes
void debounce(int valor);
void gpio_config(void);     // Configura gpio
void ta0_config(void);  // Configura timer TA0
void tb0_config(void);  // configura timer TB0
void mapear_led2(void); // Função para mapear o led verde
int mon_s1(void);   // função que monitora chave 1
int mon_s2(void);   // função que monitora chave 2
int ambas = 0;

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    mapear_led2();
    gpio_config();
    mon_s1();
    mon_s2();
    ta0_config();
    tb0_config();
//    primeiro();

    while (TRUE){
             if (mon_s1() == TRUE){
                 if (TA0CCR1 < TA0CCR0 )  {   //Limite de 100%?
                     TA0CCR1 += PASSO_5;   //+20%
                 }
                 if (TB0CCR1 > 0)   {
                     TB0CCR1 -= PASSO_7;
                 }

             }
             if (mon_s2() == TRUE){
                 if (TB0CCR1 < TBCCR0)  {          //Está em zero?
                     TB0CCR1 += PASSO_7;
                 }
                 if (TA0CCR1 > 0) {
                  TA0CCR1 -= PASSO_5;
                 }

             }
             

         }
         return 0;


}





// Função para configurar TA0.1
void ta0_config(void){
        TA0CTL =  TASSEL_2 |    //SMCLK
        MC_1;         //Modo 1 (Up)
        TA0CCTL1 = OUTMOD_6;    //Saída Modo 6
        TA0CCR0 = T_PWM;        //Período do PWM
        TA0CCR1 = 0*PASSO_5;   //Iniciar zerado
        P1DIR |= BIT2;          //P1.2 como saída
        P1SEL |= BIT2;          //P1.2 dedicado ao OUT1

}

// Função para configurar TB0.1
void tb0_config(void){
        TB0CTL = TBSSEL_2 | MC_1; //TB0 com SMCLK e Modo Up
        TB0CCR0 = T_PWM; //Período PWM
        TB0CCTL1 = OUTMOD_6; //Saída Modo 6
        TB0CCR1 = 0*PASSO_7; //Iniciar zerado

}

// Monitorar S1, retorna TRUE se foi acionada
int mon_s1(void){
    static int ps1=ABERTA;      //Guardar passado de S1
    if ( (P2IN&BIT1) == 0){     //Qual estado atual de S1?
	if(ambas == 2){
	ambas2();
	ambas = 0;
	ps1=FECHADA;
	return FALSE;
	}
        if (ps1==ABERTA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=FECHADA;
	    ambas =  1;
            return TRUE;
        }
    }
    else{
        if (ps1==FECHADA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=ABERTA;
	    ambas = 0;
            return FALSE;
        }
    }
    return FALSE;
}

// Monitorar S2, retorna TRUE se foi acionada
int mon_s2(void){
    static int ps2=ABERTA;      //Guardar passado de S2
    if ( (P1IN&BIT1) == 0){     //Qual estado atual de S2?
	if(ambas == 1){
	ambas1();
	ambas = 0;
	ps2=FECHADA;
	return FALSE;
	}
        if (ps2==ABERTA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=FECHADA;
	    ambas =  2;
            return TRUE;
        }
    }
    else{
        if (ps2==FECHADA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=ABERTA;
	    ambas =  0;
            return FALSE;
        }
    }
    return FALSE;
}



// Debounce
void debounce(int valor){
    volatile int x;
    for (x=0; x<valor; x++);
}

void ambas1(){
	TA0CCR1 = 0;
	TB0CCR1 = 0;
}

void ambas2(){
	TA0CCR1 = TA0CCR0;
	TB0CCR1 = TB0CCR0;
}
// Função para configurar leds e chaves
void gpio_config(void){
                 P1DIR |= BIT0;              //Led1 = P1.0 = saída
                 P1OUT &= ~BIT0;             //Led1 apagado

                 P4DIR |= BIT7;              //Led2 = P4.7 = saída
                 P4OUT &= ~BIT7;             //Led1 apagado

                 P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
                 P2REN |= BIT1;              //Habilitar resistor
                 P2OUT |= BIT1;              //Habilitar pullup

                 P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
                 P1REN |= BIT1;              //Habilitar resistor
                 P1OUT |= BIT1;              //Habilitar pullup
}

// Função para mapear TB0.1 no led2 (verde)
void mapear_led2(void){
                P4DIR |= BIT7;          //P4.7 = saída
                P4SEL |= BIT7;          //P4.7 = função alternativa
                PMAPKEYID = 0X02D52;    //Chave para mapeamento
                P4MAP7 = PM_TB0CCR1A;   //Mapeat TB0.1 em P4.7
}
