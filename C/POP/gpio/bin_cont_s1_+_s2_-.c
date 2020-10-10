// P2.c - SM-08-C-5-CCS
// Contador binário
// S1 incrementa
// S2 decrementa

#include <msp430.h> 

#define TRUE    1
#define FALSE   0
#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce


// Protótipo das funções
int mon_s1(void);
int mon_s2(void);
void cont_leds(int ct);
void debounce(int valor);
void io_config(void);

int main(void){
    int cont=0;
     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
     io_config();
     while (TRUE){
         if (mon_s1() == TRUE)  cont++; //Chave 1?
         if (mon_s2() == TRUE)  cont--; //Chave 2?
         cont_leds(cont);
     }
     return 0;
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


// Acionar os leds
void cont_leds(int ct){
    switch(ct&3){
        case 0: P1OUT &= ~BIT0;     P4OUT &= ~BIT7;     break;  //00
        case 1: P1OUT &= ~BIT0;     P4OUT |=  BIT7;     break;  //01
        case 2: P1OUT |=  BIT0;     P4OUT &= ~BIT7;     break;  //10
        case 3: P1OUT |=  BIT0;     P4OUT |=  BIT7;     break;  //11
    }
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
    P4OUT &= ~BIT7;     //Led1 apagado

    P2DIR &= ~BIT1;     //S1 = P2.1 = entrada
    P2REN |= BIT1;      //Habilitar resistor
    P2OUT |= BIT1;      //Habilitar pullup

    P1DIR &= ~BIT1;     //S2 = P1.1 = entrada
    P1REN |= BIT1;      //Habilitar resistor
    P1OUT |= BIT1;      //Habilitar pullup
}
