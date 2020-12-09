// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// B2p1-Esqueleto.C
//
// Conex�es LCD
// P3.0 = SDA
// P3.1 = SCL

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

// Fun��es para criar as Barras
void lcd_barras(void);
void pblcd_char(char x);
void pblcd_cmdo(char x);
void i2c_write(char x);     //<<== atualizar essa fun��o

void adc_config(void);
void ta0_config(void);

volatile int medx,medy; //Valores das m�dias

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Suas inicializa��es

    lcd_barras();   //Criar as Barras
    ta0_config();   //Configurar TA0
    adc_config();   //Configurar ADC
    __enable_interrupt();

    while(TRUE){
        //Coloque seu c�digo dentro deste la�o
        ;
    }

    return 0;
}

#pragma vector = ADC12_VECTOR    //54
__interrupt void adc_isr(void){ ; }

// Configurar ADC
void adc_config(void){ ; }

// Configurar TA0
void ta0_config(void){ ; }


//////////////////////////////////////////////////////
////// Abaixo est�o as fun��es necess�rias       /////
//////         para criar as barras              /////
//////  Voc� deve fornecer a fun��o i2c_write()  /////
//////////////////////////////////////////////////////

// Gerar barras horizontais
void lcd_barras(void){
    int i;
    pblcd_cmdo(0x40);
    for (i=0; i<64; i++)      pblcd_char(0);
    pblcd_cmdo(0x47);         pblcd_char(0x3F);
    pblcd_cmdo(0x4E);         pblcd_char(0x3F);
    pblcd_cmdo(0x55);         pblcd_char(0x3F);
    pblcd_cmdo(0x5C);         pblcd_char(0x3F);
    pblcd_cmdo(0x63);         pblcd_char(0x3F);
    pblcd_cmdo(0x6A);         pblcd_char(0x3F);
    pblcd_cmdo(0x71);         pblcd_char(0x3F);
    pblcd_cmdo(0x78);         pblcd_char(0x3F);
    pblcd_cmdo(0x80);
}

void pblcd_char(char c){
    char nib;
    nib=c&0xF0;
    i2c_write(nib|0x9);
    i2c_write(nib|0xD);
    i2c_write(nib|0x9);
    nib=(c<<4)&0xF0;
    i2c_write(nib|0x9);
    i2c_write(nib|0xD);
    i2c_write(nib|0x9);
}

void pblcd_cmdo(char c){
    char nib;
    nib=c&0xF0;
    i2c_write(nib|0x8);
    i2c_write(nib|0xC);
    i2c_write(nib|0x8);
    nib=(c<<4)&0xF0;
    i2c_write(nib|0x8);
    i2c_write(nib|0xC);
    i2c_write(nib|0x8);
}

// Fun�ao para enviar um  byte para o PCF8574
// Voc� j� deve ter esta fun��o pronta.
// Se necess�rio, seu nome pode ser alterado
void i2c_write(char c) {;}

//////////////////////////////////////////////////////
////// Deste ponto em  diante n�o ser� corrigido /////
//////    Coloque abaixo suas demais fun��es     /////
//////////////////////////////////////////////////////
