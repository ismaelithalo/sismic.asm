// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// B2p2-Esqueleto.C
//
// Conex�es LCD
// P3.0 = SDA
// P3.1 = SCL

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

#define QTD 800 //Quantidade de n�meros aleat�rios

// Vetor para facilitar a cria��o das barras
char barras[64]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,   //0
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F,   //1
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,   //2
                  0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,   //3
                  0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //4
                  0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //5
                  0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //6
                  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};  //7

// Fun��es para criar as barras
void lcd_barras(void);
void pblcd_char(char x);
void pblcd_cmdo(char x);
void i2c_write(char x);     //<<== atualizar essa fun��o

// Fun��es a serem elaboradas
char rand(void);
void vet_rand(char *vt);
int freq(char x, char *vet);
void barraxy(char x, int freq);
void adc_canal(char canal);
void ta0_config(void);
void adc_config(void);

volatile char vetor[QTD];   //Vetor para receber os n�meros aleat�rios

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    ta0_config();
    adc_config();
    lcd_barras();

    while(TRUE){
        //Coloque seu c�digo dentro deste la�o
        ;
    }
    return 0;
}

char rand(void){;}

void vet_rand(char *vt){;}

int freq(char x, char *vet){;}

void barraxy(char x, int freq){;}

void adc_canal(char canal){;}

void ta0_config(void){;}

void adc_config(void){;}


//////////////////////////////////////////////////////
////// Abaixo est�o as fun��es necess�rias       /////
//////         para criar as barras              /////
//////  Voc� deve fornecer a fun��o i2c_write()  /////
//////////////////////////////////////////////////////

void lcd_barras(void){
    int i;
    pblcd_cmdo(0x40);
    for (i=0; i<64; i++)    pblcd_char(barras[i]);
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
////// Daqui ponto em  diante n�o ser� corrigido /////
//////    Coloque abaixo suas demais fun��es     /////
//////////////////////////////////////////////////////

