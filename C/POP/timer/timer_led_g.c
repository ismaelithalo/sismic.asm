// P05.c - SM-09-Timers-8-CCS
// PWM com Timer TB0.1 --> Led Verde
// S1 incrementa 10% no ciclo de carga
// S2 decrementa 10% no ciclo de carga

#include <msp430.h> 

#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

//PWM
#define T_PWM      5240   //Período de 2x5ms com SMCLK
#define PASSO_10    524   //Passo de 10% no ciclo de carga


// Protótipo das funções
void tb0_config(void);
int mon_s1(void);
int mon_s2(void);
void debounce(int valor);
void io_config(void);

int main(void){
     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
     io_config();
     tb0_config();
     __enable_interrupt();
     while (TRUE){
         if (mon_s1() == TRUE){
             if (TB0CCR1 < TB0CCR0)     //Limite de 100%?
                 TB0CCR1 += PASSO_10;   //+10%
         }
         if (mon_s2() == TRUE){
             if (TB0CCR1 >0)            //Está em zero?
                 TB0CCR1 -= PASSO_10;   //-10%
         }
     }
     return 0;
}

// Configurar TB0, CCR1
void tb0_config(void){
    TB0CTL =  TBSSEL_2 |    //SMCLK
              MC_3;         //Modo 3 (Up/Down)
    TB0CCTL1 = OUTMOD_2 |   //Saída Modo 2
               CLLD_1;      //Atualiza qdo TB0R = 0
    TB0CCTL0 = CCIE;        //Hab Interrupt TB0.0
    TB0CCR0 = T_PWM;        //Período do PWM
    TB0CCR1 = 5*PASSO_10;   //Iniciar com 50%

    P4DIR |= BIT1|BIT7;     //P4.7 como saída
    P4SEL |= BIT1|BIT7;     //P4.7 dedicado ao OUT1

    // Mapear TB0.1 em P4.7
    PMAPKEYID = 0X02D52;    //Liberar mapeamento
    P4MAP7 = PM_TB0CCR1A;   //Sair por P4.7
    P4MAP1 = PM_TB0CCR1A;   //Sair por P4.1
}

// Interrupção TB0CCR0
#pragma vector = TIMER0_B0_VECTOR
__interrupt void isr_ccifg0(void){
    P2OUT |= BIT5;
    __no_operation();   //NOP;
    __no_operation();   //NOP;
    P2OUT &= ~BIT5;
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

    P2DIR |= BIT5;      //P2.5 saida
    P2OUT &= ~BIT5;     //P2.5 = 0
}
