// 190030194 - Ismael Ithalo Barbosa Pinto
// Prova B1, Questao 2

#include <msp430.h> 

// Aqui esta diferente das instruçoes da prova, as freqs estao j,i,h e la esta i,h,g, seguirei a prova

// Altere as linhas abaixo de acordo com sua matricula  1 9 0 0 3 0 1 9 4
#define MATR  190030194  //Escreva aqui sua matricula ( a b c d e f g h i j)
#define CCR0  1900       //abcd
#define FREQ1 4          //i Hz (se j=0, entao FREQ1 = 10 Hz)
#define FREQ2 9          //h HZ (se i=0, entao FREQ2 = 10 Hz)
#define FREQ3 1          //g Hz (se h=0, entao FREQ3 = 10 Hz)
// Configuaraçoes da leitura da chave
#define TRUE 1
#define FALSE 0
#define ABERTA 1
#define FECHADA 0
#define DBC 1000
#define DBC_led 50000

//Configuraçao, monitoriamento e uso da chave e led

int mon_s1(void);
int mon_s2(void);
void debounce(int valor);

void gpio_config(void);
void ta2_config(void);

int main(void){

            WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

            gpio_config();
            ta2_config();

            while(TRUE) {

                while ((TA2CTL&TAIFG)==0); //Esperar TAIFG=1
                TA2CTL &= ~TAIFG;
                P1OUT ^= BIT0;  //Pisca o led, mas como o timer e A2, ele ascende no dobro da frequencia inserida

                if (mon_s1() == TRUE) { //Chave 1?
                    debounce(DBC);
                    if (mon_s2() == TRUE) {   //Chave 2 junto = freq 1
                        debounce(DBC);
                        //Aqui viria a freq 1, porem nao consegui alterar a frequencia sem mexer no CCR0

                    }
                    else {                  //Freq 3

                              //Aqui viria a freq 3, porem nao consegui alterar a frequencia sem mexer no CCR0              
                        
                    }
                    
                }
                if (mon_s2() == TRUE) { //Chave 2?
                    debounce(DBC);
                    if (mon_s1() == TRUE) {     //Chave 1 junto =  freq 1
                        debounce(DBC);
                        //Aqui viria a freq 1, porem nao consegui alterar a frequencia sem mexer no CCR0

                        }
                    else {              //Freq 2

                    //Aqui viria a freq 2, porem nao consegui alterar a frequencia sem mexer no CCR0
                        
                    }
                    
                }
            }

            return 0;
}

////////////////////////////////////////////////////////////////////

// Configurar os pinos necessarios
void gpio_config(void){

            P1DIR |= BIT0;              //Led1 = P1.0 = saída
            P1OUT &= ~BIT0;             //Led1 apagado

            P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
            P2REN |= BIT1;              //Habilitar resistor
            P2OUT |= BIT1;              //Habilitar pullup

            P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
            P1REN |= BIT1;              //Habilitar resistor
            P1OUT |= BIT1;              //Habilitar pullup
}

// Configurar TA2
// Usar modo 3 (up/down)
// Usar TA2CCR=CCR0
void ta2_config(void){

            TA2CTL = TASSEL_1 | MC_3;   //ACLK = 32.768 Hz e Modo 3, up/down
            TA2CCR0 = CCR0;             //CCRO = 1900 
            /*
            0,1 seg = 3276 no ACLK
            0,04 seg = 1310
            0,09 seg = 2948
            0,01 seg = 327,6
            Porem no modo 3 o clock eh dobrado entao deve-se dividir os valores
            dependendo das condiçoes
            */
}

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
