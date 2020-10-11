// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Quest�o 4

#include <msp430.h> 

// Prot�tipo de sua fun��o para configurar TA1
void ta1_config(void);

//Prot�tipo das fun��es b�sicas
void rlg_liga(void);
void rlg_sobe(void);
void rlg_desce(void);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	rlg_liga();
	ta1_config();

	// seu c�digo
	
	return 0;
}

// Sua fun��o para configurar TA1
void ta1_config(void){;}

////////////////////////////////////////////
// A seguir est�o as fun��es b�sicas      //
// N�o altere as fun��es abaixo           //
////////////////////////////////////////////

// Ligar RLG (TA2.1)
void rlg_liga(void){
    TA2CTL = TASSEL_1|ID_0|MC_3;
    TA2EX0 = TAIDEX_1;
    TA2CCR0 = 100;
    TA2CCR1= TA2CCR0/2;
    TA2CCTL1 = OUTMOD_4;
    P2DIR |= BIT4;
    P2SEL |= BIT4;
}

// Aumentar frequ�ncia de RLG em 1 passo
void rlg_sobe(void){
    TA2CCR0--;
    TA2CCR1= TA2CCR0/2;
}

// Diminuir frequ�ncia de RLG em 1 passo
void rlg_desce(void){
    TA2CCR0++;
    TA2CCR1= TA2CCR0/2;
}


