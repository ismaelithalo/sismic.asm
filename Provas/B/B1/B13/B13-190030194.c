// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Questao 3

#include <msp430.h>

#define FREQ_PWM 1900       //(Hz) Quatro primeiros digitos de sua matricula

// Configuaraçoes da leitura da chave
#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

//PWM
/*
	p= 1/1900 * 1.048.576 = 551
*/
#define T_PWM 551     //Período calculado a partir da matricula
#define PASSO_5 110   //p/5
#define PASSO_7 78   // p/7

// Pelo menos essas 3 funçoes precisam estar presentes
void gpio_config(void);
void ta0_config(void);
void tb0_config(void);
void mapear_led2(void);	// Funçao para mapear o led verde

//Configuraçao, monitoriamento e uso da chave e led

int mon_s1(void);
int mon_s2(void);
void debounce(int valor);

int main(void){

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	mapear_led2();
	gpio_config();
	ta0_config();
	tb0_config();

	//Snippet retirado do ex P05.c - SM-09-Timers-8-CCS
	while (TRUE){
         if (mon_s1() == TRUE){
             if (TA0CCR1 < TA0CCR0){     //Limite de 100%?
                 TA0CCR1 += PASSO_5;   
				 TB0CCR1 -= PASSO_7;
         	}
		 }
         if (mon_s2() == TRUE){
             if (TB0CCR1 < TBCCR0){            //Está em zero?
                 TB0CCR1 += PASSO_7;
				 TA0CCR1 -= PASSO_5; 
         	}
		 }
     }

	return 0;
}

// Funçao para configurar TA0.1 retirada do exemplo P05.c - SM-09-Timers-8-CCS
void ta0_config(void){
	TA0CTL =  TASSEL_2 | MC_1;// SMCLK Modo 1 (Up)
    TA0CCTL1 = OUTMOD_6;    //Saída Modo 6
    TA0CCR0 = T_PWM;        //Período do PWM
    TA0CCR1 = 5*PASSO_5;   //Inicia no maximo
    P1DIR |= BIT2;          //P1.2 como saída
    P1SEL |= BIT2;          //P1.2 dedicado ao OUT1
}

// Funçao para configurar TB0.1 retirada do exemplo P06.c - SM-09-Timers-9-CCS
void tb0_config(void){
	TB0CTL =  TBSSEL_2 | MC_1; // SMCLK Modo 1 (Up)
    TB0CCTL1 = OUTMOD_6;    //Saída Modo 6 para ter o mesmo comportamento do TA0
    TB0CCR1 = T_PWM;        //Período do PWM
    TB0CCR1 = 7*PASSO_7;   //Diferente do exemplo aqui sao 7 passos
    
	//Diferente do exemplo TB0.1 esta mapeado na 4.7, entao algumas adaptaçoes foram feitas
}

// Funçao para configurar leds e chaves
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

// Funçao para mapear TB0.1 no led2 (verde)
void mapear_led2(void){
    P4DIR |= BIT7;          //P4.7 = saida
    P4SEL |= BIT7;          //P4.7 = funçao alternativa
    PMAPKEYID = 0X02D52;    //Chave para mapeamento
    P4MAP7 = PM_TB0CCR1A;   //Mapeia TB0.1 em P4.7
}

////////////////////////////////////////////////////////////////////

//Funçao de atraso
void debounce(int valor){
             volatile int x; //volatile evita optimizador do compilador
                 for (x=0; x<valor; x++); //Apenas gasta tempo
}

//Monitoramento da chave s1
int mon_s1(void){
            static int ps1=ABERTA; //Guardar passado de S1
                if ( (P2IN&BIT1) == 0){ //Qual estado atual de S1?
                    if (ps1==ABERTA){ //Qual o passado de S1?
                        debounce(DBC);
                        ps1=FECHADA;
                        return TRUE;
                    }
    }
                else{
                    if (ps1==FECHADA){            //Qual o passado de S1?
                        debounce(DBC);
                        ps1=ABERTA;
                        return FALSE;
                }
            }
            return FALSE;
    }

//Monitoramento da chave s2
int mon_s2(void){
            static int ps2=ABERTA; //Guardar passado de S2
                if ( (P1IN&BIT1) == 0){ //Qual estado atual de S2?
                    if (ps2==ABERTA){ //Qual o passado de S2?
                        debounce(DBC);
                        ps2=FECHADA;
                        return TRUE;
                }
            }
                else{
                    if (ps2==FECHADA){ //Qual o passado de S2?
                        debounce(DBC);
                        ps2=ABERTA;
                        return FALSE;
                }
            }
            return FALSE;
    }