//190030194 - Ismael Ithalo Barbosa Pinto

#include <msp430.h> 

int mon_s1(void);
void io_config(void);
void timer_config(void);
void debounce(int valor);
void calcula_sinaliza(int x, int y);
void clockInit();

// Configuaraçoes da leitura da chave
#define TRUE 1
#define FALSE 0
#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000
#define DLY 10  // Duraçao maxima do trigger: 10 µs

volatile int dist; // Variavel global que recebe a distancia

void main(void)
{
        int ida, volta;

	    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

	    io_config();
        clockInit();
        timer_config();

	     while(TRUE) {
            if (mon_s1() == TRUE) { 
                while (mon_s1() != TRUE) {
                    P4OUT |= BIT7;
                    P1OUT &= ~BIT0;
                }
                P4OUT &= ~BIT7;
                P1OUT |= BIT0; 
            }
	    }

}

//Funçao de atraso
void debounce(int valor){
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

//Configurando timer para captura
void timer_config(void){

	        TA0CTL =    TASSEL_2 |  // SMCLK
                        ID_0 |      //
                        MC_2;       // Modo 2 = Continuous 

            TA0CCTL1 =  CM_1|       //
                        CCIS_0|     //
                        SCS|        //
                        CAP;        //
	    }

//Funçao que calcula a distancia e sinaliza com os leds
void calcula_sinaliza(int x, int y){
            volatile int diferenca;
            /*
                (S-So)=(340 m/s * (t-to) µs )/2
                A distancia se da pela mult. da velocidade do som pela diferença
                de tempo calculado. Como queremos os resultados em centimetros e a
                diferença vem em µs, devemos fazer alguma correçoes.
                (t-to) µs = (t-to)/1000000 s
                340 m/s = 34000 cm/s
            */
            diferenca = (y - x)/1000000;        //(t-to) µs = (t-to)/1000000 s
            dist = (34000*diferenca)/2;         // cm = (cm/s)*s

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

// Funçao de ajuste do clock
void clockInit() {

    // Configure crystal ports
    P5SEL |= BIT2 | BIT3 | BIT4 | BIT5;     // Configure P5 to use Crystals

    // This should make XT1 startup in 500ms and XT2 in less than 1ms
    UCSCTL6 = XT2DRIVE_3      |             // Turn up crystal drive to
              XT1DRIVE_3      |             // speed up oscillator startup
              XCAP_3          |             // Use maximum capacitance (12pF)
//            XT1OFF          |             // Make sure XT1 and XT2
//            XT2OFF          |             // oscillators are active
//            SMCLKOFF        |             // Leave SMCLK ON
//            XT1BYPASS       |             // Not using external clock source
//            XT2BYPASS       |             // Not using external clock source
//            XTS             |             // XT1 low-frequency mode, which
              0;                            // means XCAP bits will be used

    UCSCTL0 = 0x00;                         // Let FLL manage DCO and MOD

    UCSCTL1 = DCORSEL_5       |             // Select DCO range to around 16MHz
//            DISMOD          |             // Enable modulator
              0;

    UCSCTL2 = FLLD__1         |             // Set FLL dividers
              FLLN(4);                      // DCOCLK = 1 * 4 * FLLREF

    UCSCTL3 = SELREF__XT2CLK  |             // Use XT2 Oscillator for FLLREF
              FLLREFDIV__1;                 // divided by 1

    UCSCTL5 = DIVPA__1        |             // Output dividers to 1
               DIVA__1        |             // ACLK  divided by 1
               DIVS__4        |             // SMCLK divided by 4
               DIVM__1;                     // MCLK  divided by 1

    UCSCTL7 = 0;                            // Clear XT2,XT1,DCO fault flags

    UCSCTL8 = SMCLKREQEN      |             // Enable conditional requests for
               MCLKREQEN      |             // SMCLK, MCLK and ACLK. In case one
               ACLKREQEN;                   // fails, another takes over

    do {                                    // Check if all clocks are oscillating
      UCSCTL7 &= ~(   XT2OFFG |             // Try to clear XT2,XT1,DCO fault flags,
                    XT1LFOFFG |             // system fault flags and check if
                       DCOFFG );            // oscillators are still faulty
      SFRIFG1 &= ~OFIFG;                    //
    } while (SFRIFG1 & OFIFG);              // Exit only when everything is ok

    UCSCTL6 &= ~(XT1DRIVE_3   |             // Xtal is now stable,
                 XT2DRIVE_3);               // reduce drive strength (to save power)

    UCSCTL4 = SELA__XT1CLK    |             // ACLK  = XT1   =>      32.768 Hz
              SELS__XT2CLK    |             // SMCLK = XT2/4 =>   1.000.000 Hz
              SELM__DCOCLK;                 // MCLK  = DCO   =>  16.000.000 Hz
}