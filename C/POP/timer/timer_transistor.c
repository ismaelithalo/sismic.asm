// P06.c - SM-09-Timers-9-CCS
// PWM com Timer TB0.6 (P3.6) --> Transistor
// S1 incrementa 10% no ciclo de carga
// S2 decrementa 10% no ciclo de carga
// Led Vermelho = 100% e
// Led Verde = 0%

#include <msp430.h> 

#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

//PWM
#define T_PWM       1050  //Período de 1ms com SMCLK
#define PASSO_10    105   //Passo de 10% no ciclo de carga

// Protótipo das funções
void tb0_config(void);
int mon_s1(void);
int mon_s2(void);
void debounce(int valor);
void io_config(void);

int main(void){
    int cont=0;
     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
     io_config();
     tb0_config();
     while (TRUE){
         if (mon_s1() == TRUE){
             if (TB0CCR6 < TB0CCR0)     //Limite de 100%?
                 TB0CCR6 += PASSO_10;   //+10%
         }
         if (mon_s2() == TRUE){
             if (TB0CCR6 >0)            //Está em zero?
                 TB0CCR6 -= PASSO_10;   //-10%
         }
         if (TB0CCR6 == TB0CCR0)    P1OUT |= BIT0;  //VM aceso
         else if (TB0CCR6 == 0)     P4OUT |= BIT7;  //VD aceso
         else{                      P1OUT &= ~BIT0; //VM apagado
                                    P4OUT &= ~BIT7; //VDM apagado
         }
     }
     return 0;
}

// Configurar TB0, CCR6
void tb0_config(void){
    TB0CTL =  TBSSEL_2 |    //SMCLK
              MC_1;         //Modo 1 (Up)
    TB0CCTL6 = OUTMOD_3;    //Saída Modo 3
    TB0CCR0 = T_PWM;        //Período do PWM
    TB0CCR6 = 5*PASSO_10;   //Iniciar com 50%
    P3DIR |= BIT6;          //P3.6 como saída
    P3SEL |= BIT6;          //P3.6 dedicado ao OUT6
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

    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led2 apagado

    P2DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P2REN |= BIT1;      //Habilitar resistor
    P2OUT |= BIT1;      //Habilitar pullup

    P1DIR &= ~BIT1;     //S2 = P1.1 = entrada
    P1REN |= BIT1;      //Habilitar resistor
    P1OUT |= BIT1;      //Habilitar pullup
}