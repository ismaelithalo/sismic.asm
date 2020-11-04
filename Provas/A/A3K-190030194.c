// 190030194 - Ismael ithalo  Barbosa Pinto
// Prova A3
#include <msp430.h> 

// Protótipo de suas funções
char ler_escravo(char adr);
void USCI_B0_config(void);
void io_config(void);

// Funções originais
void USCI_B1_config(void);
void USCI_B1_rx(void);
void USCI_B1_tx(void);  //<<==alterar esta função

// Configuaraçoes da leitura da chave
#define TRUE 1
#define FALSE 0
#define ABERTA 1
#define FECHADA 0
#define DBC 1000

//Configuraçao, monitoriamento e uso da chave e led

int mon_s1(void);
int mon_s2(void);
void debounce(int valor);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    USCI_B1_config();
    io_config();

        while(TRUE) {

                    if (mon_s1() == TRUE) { //Chave 1?
                        debounce(DBC);
                        if (mon_s2() == TRUE) {   //Chave 2 junto
                            debounce(DBC);
                            P1OUT ^= BIT0;
                            P4OUT ^= BIT7;
                        }
                        else {

                            P1OUT ^= BIT0;
                            P4OUT &= ~BIT7;
                        }
                        
                    }
                    if (mon_s2() == TRUE) { //Chave 2?
                        debounce(DBC);
                        if (mon_s1() == TRUE) {     //Chave 1 junto
                            debounce(DBC);
                            P1OUT ^= BIT0;
                            P4OUT ^= BIT7;
                            }
                        else {

                            P4OUT ^= BIT7;
                            P1OUT &= ~BIT0;
                        }
                        
                    }
                }

    return 0;
}

// Funções a serem escritas
char ler_escravo(char adr){;}
void USCI_B0_config(void){;}

// Configurar os pinos necessarios
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

/////////////////////////////////////////////////////
//////////////// Funções do ER 9.1 //////////////////
/////////////////////////////////////////////////////

// Esta função precisa ser alterada
void USCI_B1_tx(void){
    UCB1TXBUF = 0x55;
}

// Não alterar esta função
void USCI_B1_config(void){
    UCB1CTL1 = UCSWRST;
    UCB1CTL0 = UCMODE_3 | UCSYNC;
    UCB1I2COA = 37;
    UCB1CTL1 = 0;
    UCB1IE = UCTXIE | UCRXIE;
    P4SEL |= BIT2 | BIT1;
    P4REN |= BIT2 | BIT1;
    P4OUT |= BIT2 | BIT1;
    __enable_interrupt();
}

// Não alterar esta função
#pragma vector = USCI_B1_VECTOR         //Vetor 45
__interrupt void isr_USCI_B1(void){
    int n;
    n = __even_in_range(UCB1IV,0xC);    //Consultar UCB1IV
    switch(n){
        case 0x0: break;            case 0x2: break;
        case 0x4: break;            case 0x6: break;
        case 0x8: break;
        case 0xA: USCI_B1_rx();  break; //Rotina RX
        case 0xC: USCI_B1_tx();  break; //Rotina TX
    }
}

// Não alterar esta função
void USCI_B1_rx(void){ ; }

//Atraso comum
void debounce(int valor){
             volatile int x;
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
