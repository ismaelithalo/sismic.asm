// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Quest�o 2

#include <msp430.h> 

// Aqui esta diferente das instruçoes da prova, as freqs estao j,i,h e la esta i,h,g, seguirei a prova

// Altere as linhas abaixo de acordo com sua matr�cula	1 9 0 0 3 0 1 9 4
#define MATR  190030194  //Escreva aqui sua matr�cula ( a b c d e f g h i j)
#define CCR0  1900       //abcd
#define FREQ1 4          //i Hz (se j=0, ent�o FREQ1 = 10 Hz)
#define FREQ2 9          //h HZ (se i=0, ent�o FREQ2 = 10 Hz)
#define FREQ3 1          //g Hz (se h=0, ent�o FREQ3 = 10 Hz)

// Prot�tipo de suas fun��es
void gpio_config(void);
void ta2_config(void);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	gpio_config();
	ta2_config();
	
	// Seu c�digo
	
	return 0;
}


// Configurar os pinos necess�rios
void gpio_config(void){;}

// Configurar TA2
// Usar modo 3 (up/down)
// Usar TA2CCR=CCR0
void ta2_config(void){;}
