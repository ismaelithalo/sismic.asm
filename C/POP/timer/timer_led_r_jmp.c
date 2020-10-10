// P05.c - SM-09-Timers-8-CCS
// PWM com Timer TA0.1 --> Led Vermelho
// S1 incrementa 10% no ciclo de carga
// S2 decrementa 10% no ciclo de carga
// Usar cabo para ligar P1.2 (TA0.1) --> JP8.2

#include <msp430.h> 

#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

//PWM
#define T_PWM 10480     //Período de 10ms com SMCLK
#define PASSO_10 1048   //Passo de 10% no ciclo de carga


// Protótipo das funções
void ta0_config(void);
int mon_s1(void);
int mon_s2(void);
void debounce(int valor);
void io_config(void);

int main(void){
    int cont=0;
     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
     io_config();
     ta0_config();
     while (TRUE){
         if (mon_s1() == TRUE){
             if (TA0CCR1 < TA0CCR0)     //Limite de 100%?
                 TA0CCR1 += PASSO_10;   //+10%
         }
         if (mon_s2() == TRUE){
             if (TA0CCR1 >0)            //Está em zero?
                 TA0CCR1 -= PASSO_10;   //-10%
         }
     }
     return 0;
}

// Configurar TA0, CCR1
void ta0_config(void){
    TA0CTL =  TASSEL_2 |    //SMCLK
              MC_1;         //Modo 1 (Up)
    TA0CCTL1 = OUTMOD_6;    //Saída Modo 6
    TA0CCR0 = T_PWM;        //Período do PWM
    TA0CCR1 = 5*PASSO_10;   //Iniciar com 50%
    P1DIR |= BIT2;          //P1.2 como saída
    P1SEL |= BIT2;          //P1.2 dedicado ao OUT1
}

// Monitorar S1, retorna TRUE se foi acionada
int mon_s1(void){
    static int ps1=ABERTA;      //Guardar passado de S1
    if ( (P2IN&BIT1) == 0){     //Qual estado atual de S1?
        if (ps1==ABERTA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps1==FECHADA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

// Monitorar S2, retorna TRUE se foi acionada
int mon_s2(void){
    static int ps2=ABERTA;      //Guardar passado de S2
    if ( (P1IN&BIT1) == 0){     //Qual estado atual de S2?
        if (ps2==ABERTA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps2==FECHADA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=ABERTA;
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

// Configurar GPIO
void io_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado

    P2DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P2REN |= BIT1;      //Habilitar resistor
    P2OUT |= BIT1;      //Habilitar pullup

    P1DIR &= ~BIT1;     //S2 = P1.1 = entrada
    P1REN |= BIT1;      //Habilitar resistor
    P1OUT |= BIT1;      //Habilitar pullup
}
