#include <msp430.h> 


/**
 * main.c
 */

#define TRUE 1
#define FALSE 0
#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000

int mon_s1(void);
int mon_s2(void);
void io_config(void);
void debounce(int valor);


void main(void)
{
	
	WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
	    io_config();

	     while(TRUE) {
	         if (mon_s1() == TRUE) {     //Chave 1?
	             debounce(DBC);
	             P1OUT ^= BIT0;
	         }
	         if (mon_s2() == TRUE) {     //Chave 2?
	             debounce(DBC);
	             P1OUT ^= BIT0;
	         }
	         }

}

void debounce(int valor){
	         volatile int x; //volatile evita optimizador do compilador
	             for (x=0; x<valor; x++); //Apenas gasta tempo
	         }

void io_config(void){

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


	     // Monitorar S1, retorna TRUE se foi acionada
int mon_s1(void){
	             static int ps1=ABERTA; //Guardar passado de S1
	                    if ( (P2IN&BIT1) == 0){ //Qual estado atual de S1?
	                        if (ps1==ABERTA){ //Qual o passado de S1?
	                         debounce(DBC);
	                         ps1=FECHADA;
	                         return TRUE;
	      }
	      }
	                    else    {
	                        if (ps1==FECHADA){            //Qual o passado de S1?
	                         debounce(DBC);
	                         ps1=ABERTA;
	                         return FALSE;
	                                    }
	                         }
	      return FALSE;
	     }

	     // Monitorar S1, retorna TRUE se foi acionada
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
