//190030194 - Ismael Ithalo Barbosa Pinto

#include <msp430.h>

int mon_s1(void);
void io_config(void);
void timer_config(void);
void atraso(int valor);
void calcula_sinaliza(int x, int y);

// Configuaraçoes da leitura da chave
#define TRUE 1
#define FALSE 0
#define ABERTA 1
#define FECHADA 0
#define DBC 1000

#define DLY 10  // Duraçao maxima do trigger: 10 µs

volatile int dist; // Variavel global que recebe a distancia

void main(void)
{
        volatile int ida, volta;

        WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

        io_config();
        timer_config();

         while(TRUE) {  //Laço infinito
            if (mon_s1() == TRUE) { //Se a chave 1 for apertada inicia o programa
                while (mon_s1() != TRUE) { //Roda a rotina enquanto a chave nao for apertda novamente

                    P1OUT |= BIT3;                          //Manda o sinal para o sensor
                    atraso(DLY);                            // Atraso de 10 µs
                    P1OUT &= ~BIT3;                         //Finaliza o sinal para o sensor
                    TA0R = 0;                               //Zera o contador do clock

                    while((TA0CCTL1 & CCIFG) == 0);         //Pulling que verifica constantemente o flanco

                        ida = TA0CCR1;                      //Captura
                        TA0CCTL1 &= ~CCIFG;                 //Reseta o alerta do flanco
                        TA0CCTL1 = CM_2|CCIS_0|SCS|CAP;     //Prepara novamente a captura

                    while((TA0CCTL1 & CCIFG) == 0);         //Pulling que verifica constantemente o flanco

                        volta = TA0CCR1;                    //Captura
                        calcula_sinaliza(ida, volta);          
                        TA0CCTL1 &= ~CCIFG;                 //Reseta o alerta do flanco
                        TA0CCTL1 = CM_1|CCIS_0|SCS|CAP;     //Prepara novamente a captura

                }
                    P4OUT &= ~BIT7;     //Desliga os leds caso tenham ficado acesos
                    P1OUT &= ~BIT0;
            }
        }

}

//Funçao de atraso
void atraso(int valor){
        volatile int x;
            for (x=0; x<valor; x++);    //Apenas gasta tempo
        }

//Configurando os pinos e leds
void io_config(void){

            P1DIR |= BIT0;              // Configura led vermelho
            P1OUT &= ~BIT0;             // Apaga vermelho (Led 1)

            P4DIR |= BIT7;              // Configura led verde
            P4OUT &= ~BIT7;             // Apaga verde (Led 2)

            P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
            P2REN |= BIT1;              //Habilitar resistor
            P2OUT |= BIT1;              //Habilitar pullup

            P1DIR &= ~BIT2;             // Entrada do sinal na placa - Echo
            P1REN |= BIT2;              // Habilitar resisitor
            P1OUT |= BIT2;              // Liga resistor de pull up
            P1SEL |= BIT2;              // Funçao extra do pino

            P1DIR |= BIT3;              // Saida do sinal da placa - Trig
            P1OUT &= ~BIT3;             // Mesmo com o bit setado para saida, coloca-se 0 por enquanto
        }

//Monitoramento da chave s1
int mon_s1(void){
        static int ps1=ABERTA; //Guardar passado de S1
            if ( (P2IN&BIT1) == 0){ //Qual estado atual de S1?
                if (ps1==ABERTA){ //Qual o passado de S1?
                    atraso(DBC);
                    ps1=FECHADA;
                    return TRUE;
                }
            }
            else    {
                if (ps1==FECHADA){            //Qual o passado de S1?
                    atraso(DBC);
                    ps1=ABERTA;
                    return FALSE;
                            }
                    }
          return FALSE;
    }

//Configurando timer para captura
void timer_config(void){

            TA0CTL =    TASSEL_2 | ID_0 | MC_2;       // Modo 2 = Continuous, ACLK
    
            TA0CCTL1 =  CM_1 | CCIS_0 | SCS | CAP;     //Configurando a captura
        }

//Funçao que calcula a distancia e sinaliza com os leds
void calcula_sinaliza(int x, int y){
            volatile double diferenca;
            /*
                (S-So)=(340 m/s * (t-to) µs )/2
                A distancia se da pela mult. da velocidade do som pela diferença
                de tempo calculado. Como queremos os resultados em centimetros e a
                diferença vem em µs, devemos fazer alguma correçoes.
                (t-to) µs = (t-to)/1000000 s
                340 m/s = 34000 cm/s
                Fazendo as contas isoladas, nao tava funcionando, por isso tive que simplificar
                cortando tres zeros de cada operaçao
            */
            diferenca = (y - x)/1000;        //(t-to) µs = (t-to)/1000000 s
            dist = (34*diferenca)/2;         // cm = (cm/s)*s

            if(dist < 20){                      // Distancia menor que 20
                P1OUT &= ~BIT0;                     // Apaga vermelho   (Led 1)
                P4OUT &= ~BIT7;                     // Apaga verde      (Led 2)
            }
            else if(dist >= 20 && dist < 40){   // Distancia de 20 ate 40
                P1OUT &= ~BIT0;                     // Apaga vermelho   (Led 1)
                P4OUT |= BIT7;                      // Acende verde     (Led 2)
            }
            else if(dist >= 40 && dist <= 60){   // Distancia de 40 ate 60
                P1OUT |= BIT0;                      // Acende vermelho  (Led 1)
                P4OUT &= ~BIT7;                     // Apaga verde      (Led 2)
            }
            else{                               // Distancia maior do que 60
                P1OUT |= BIT0;                      // Acende vermelho  (Led 1)
                P4OUT |= BIT7;                      // Acende verde     (Led 2)
            }
}
