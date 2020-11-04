// T11p02.c
//
// Básico para operar com LCD
// P3.0 = SDA
// P3.1 = SCL
//
// Comandar o LCD
//  P7  P6  P5  P4  P3  P2  P1  P0
//  D7  D6  D5  D4  BL  E   RW  RS

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

// Bits para controle do LCD
#define BIT_RS   BIT0
#define BIT_RW   BIT1
#define BIT_E    BIT2
#define BIT_BL   BIT3

# define BR10K  105 //(SMCLK) 1.048.576/105 ~= 10kHz
# define BR100K  11 //(SMCLK) 1.048.576/11  ~= 100kHz

void lcd_inic(void);
void lcd_aux(char dado);
void i2c_write(char dado);
char i2c_read(void);
char i2c_test(char adr);
void USCI_B0_config(void);
void leds_config(void);
void atraso(long x);

int main(void){
    char adr;       //Endereço do PCF_8574
    char col,base=0,i=0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    leds_config();              //Leds
    USCI_B0_config();           //Configurar USCI_B0

    if      (i2c_test(0x27) == TRUE) adr=0x27;
    else if (i2c_test(0x3F) == TRUE) adr=0x3F;
    else{
        P1OUT |= BIT0;  //Sinalizar problema
        while(TRUE);    //Travar execução
    }
    P4OUT |= BIT7;      //Verde aceso = OK
    UCB0I2CSA = adr;    //Endereço a ser usado
    i2c_write(0);       //Todos os pinos em 0
    lcd_inic();
/////////////////////////////
// LCD pronto para operar  //
// Colocar aqui seu código //
/////////////////////////////
    while(TRUE);
    return 0;
}

// Incializar LCD modo 4 bits
void lcd_inic(void){

    // Preparar I2C para operar
    UCB0CTL1 |= UCTR    |   //Mestre TX
                UCTXSTT;    //Gerar START
    while ( (UCB0IFG & UCTXIFG) == 0);          //Esperar TXIFG=1
    UCB0TXBUF = 0;                              //Saída PCF = 0;
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);   //Esperar STT=0
    if ( (UCB0IFG & UCNACKIFG) == UCNACKIFG)    //NACK?
                while(1);

    // Começar inicialização
    lcd_aux(0);     //RS=RW=0, BL=1
    atraso(20000);
    lcd_aux(3);     //3
    atraso(10000);
    lcd_aux(3);     //3
    atraso(10000);
    lcd_aux(3);     //3
    atraso(10000);
    lcd_aux(2);     //2

    // Entrou em modo 4 bits
    lcd_aux(2);     lcd_aux(8);     //0x28
    lcd_aux(0);     lcd_aux(8);     //0x08
    lcd_aux(0);     lcd_aux(1);     //0x01
    lcd_aux(0);     lcd_aux(6);     //0x06
    lcd_aux(0);     lcd_aux(0xF);   //0x0F

    while ( (UCB0IFG & UCTXIFG) == 0)   ;          //Esperar TXIFG=1
    UCB0CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
    atraso(50);
}

// Auxiliar inicialização do LCD (RS=RW=0)
// *** Só serve para a inicialização ***
void lcd_aux(char dado){
    while ( (UCB0IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB0TXBUF = ((dado<<4)&0XF0) | BIT_BL;          //PCF7:4 = dado;
    atraso(50);
    while ( (UCB0IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB0TXBUF = ((dado<<4)&0XF0) | BIT_BL | BIT_E;  //E=1
    atraso(50);
    while ( (UCB0IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB0TXBUF = ((dado<<4)&0XF0) | BIT_BL;          //E=0;
}

// Gerar START e STOP para colocar PCF em estado conhecido
void pcf_stt_stp(void){
    int x=0;
    //UCB0I2CSA = PCF_ADR;        //Endereço Escravo

    while (x<5){
        UCB0CTL1 |= UCTR    |   //Mestre TX
                    UCTXSTT;    //Gerar START
        while ( (UCB0IFG & UCTXIFG) == 0);  //Esperar TXIFG=1
        UCB0CTL1 |= UCTXSTP;                //Gerar STOP
        atraso(200);
        if ( (UCB0CTL1 & UCTXSTP) == 0)   break;   //Esperar STOP
        x++;
    }
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);   //I2C Travado (Desligar / Ligar)
}

// Escrever dado na porta
void i2c_write(char dado){
    UCB0CTL1 |= UCTR    |       //Mestre TX
                UCTXSTT;        //Gerar START
    while ( (UCB0IFG & UCTXIFG) == 0)   ;          //Esperar TXIFG=1
    UCB0TXBUF = dado;                              //Escrever dado
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT)   ;   //Esperar STT=0
    if ( (UCB0IFG & UCNACKIFG) == UCNACKIFG){      //NACK?
        P1OUT |= BIT0;      //NACK=Sinalizar problema
        while(1);           //Travar execução
    }
    UCB0CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
}

// Ler a porta do PCF
char i2c_read(void){
    char dado;
    UCB0CTL1 &= ~UCTR;                  //Mestre RX
    UCB0CTL1 |= UCTXSTT;                //Gerar START
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);
    if ( (UCB0IFG & UCNACKIFG) == UCNACKIFG){ //NACK?
        P1OUT |= BIT0;      //NACK=Sinalizar problema
        while(1);           //Travar execução
    }
    UCB0CTL1 |= UCTXSTP;                //Gerar STOP + NACK
    while ( (UCB0IFG & UCRXIFG) == 0);  //Esperar RX
    dado=UCB0RXBUF;
    while( (UCB0CTL1&UCTXSTP) == UCTXSTP);
    return dado;
}

// Testar o endereço adr para escrita
char i2c_test(char adr){
    UCB0I2CSA = adr;                //Endereço a ser testado
    UCB0CTL1 |= UCTR;               //Mestre TX --> escravo RX
    UCB0CTL1 |= UCTXSTT;            //Gerar STASRT
    while ( (UCB0IFG & UCTXIFG) == 0);  //TXIFG indica que já iniciou o START
    UCB0CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);      //Esperar STOP
    if ((UCB0IFG & UCNACKIFG) == 0) return TRUE;   //Chegou ACK
    else                            return FALSE;  //Chegou NACK
}

// Configurar USCI_B0 como mestre
// P3.0 = SDA e P3.1 = SCL
void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;     //Ressetar USCI_B1
    UCB0CTL0 = UCMST    |   //Modo Mestre
               UCMODE_3 |   //I2C
               UCSYNC;      //Síncrono
    //UCB0BRW = BR10K;      //10 kbps
    UCB0BRW = BR100K;       //100 kbps
    UCB0CTL1 = UCSSEL_3;    //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0;   //Funções alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}

// Configurar I/O
void leds_config(void){
    P1DIR |= BIT0; P1OUT &= ~BIT0; //Led Vermelho
    P4DIR |= BIT7; P4OUT &= ~BIT7; //Led Verde
}

// Atraso
void atraso(long x){
    volatile long cont=0;
    while(cont++ < x) ;
}
