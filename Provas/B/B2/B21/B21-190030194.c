// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// Conex�es LCD
// P3.0 = SDA
// P3.1 = SCL

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

// Bits para controle do LCD
#define BIT_RS   BIT0
#define BIT_RW   BIT1
#define BIT_E    BIT2
#define BIT_BL   BIT3

#define BR10K  105 //(SMCLK) 1.048.576/105 ~= 10kHz
#define BR100K  11 //(SMCLK) 1.048.576/11  ~= 100kHz

#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000

// Fun��es para criar as Barras
void lcd_barras(void);
void pblcd_char(char x);
void pblcd_cmdo(char x);
void i2c_write(char dado);

void adc_config(void);
void ta0_config(void);

void blocks();

void io_config();
void lcd_inic(void);
void lcd_aux(char dado);
char i2c_test(char adr);
void USCI_B0_config(void);
void atraso(long x);
void lcd_adr();

void lcd_write_nibble(char nib, int inst);
void lcd_write_char(char dado, int inst);
void lcd_clear();
void lcd_set_cursor(int pos);
void lcd_line_2();
void lcd_cursor(int state);

void debounce(int valor);
int mon_s3(void);
void io_config();

volatile int FREQ = 1500;           // Envia o trigger aproximadamente a cada 1/10 segundos

volatile int medx,medy;             //Valores das m�dias
volatile unsigned long int can0=0, can1=0;
volatile int flag = 0, i=0,modo=0; 

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    USCI_B0_config();           //Configurar USCI_B0
    
    io_config();                //Leds e chave no 6.2
    lcd_adr();                  // Testa o endereço a ser usado
    i2c_write(0);               //Todos os pinos em 0
    lcd_inic();
    lcd_clear();

    lcd_barras();               //Criar as Barras
    ta0_config();               //Configurar TA0
    adc_config();               //Configurar ADC
    __enable_interrupt();

    lcd_cursor(0);

    while(TRUE){
        if (mon_s3() == TRUE) {     // Chave s3?
            if(modo)                // Se o modo for 1, ele vai pra 0
                modo=0;
            else                    // Caso contrário, vá para 1
                modo=1;
        }
        blocks();                   // Chama a função que usa as variáveis globais para mostrar a barra
    }

    return 0;
}

#pragma vector = ADC12_VECTOR    //54
__interrupt void adc_isr(void){ 
        can0 += ADC12MEM0;                      // Soma a medicao de X a variavel can0
        can1 += ADC12MEM1;                      // Soma a medicao de Y a variavel can1
        i++;                                    // Incrementa i (contador de conversões)

        if (modo == 0){                         // Se o modo é zero, ele entra direto sem verificar a quantidade de conversões, pois é 1
            P1OUT &= ~BIT0;                     // Led indicativo de modo

            medx = can0;                        // Pega o valor das conversões
            medy = can1;
            flag = 1;
            can0 = 0;                           // Prepara essas variaveis para a proxima soma
            can1 = 0;
            i = 0;                              // Zera o i
            FREQ = 1500;                        // Define a frequencia para 1/10 s
            ta0_config();
        }
        else if (modo == 1) {                   // Se o modo é um, ele espera até haver 16 conversões para fazer a média
            P1OUT |= BIT0;                      // Led indicativo de modo

            if(i == 16){                        // Quando as 8 amostras de cada ja tiverem sido colhidas
                medx = can0/16;                 // Calcula a media de X
                medy = can1/16;                // Calcula a media de Y
                flag = 1;                       // Seta a flag para indicar isso
                can0 = 0;                       // Prepara essas variaveis para a proxima soma
                can1 = 0;
                i = 0;                          // Zera o i
                FREQ = 500;                     // Define a frequencia para 1/30 s
                ta0_config();
            }
        }
 }

#pragma vector = TIMER0_A0_VECTOR                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                     // Quando o CCR0 chega ao maximo, acontece a interrupcao
{
    ADC12CTL0  &=  ~ADC12SC;                        // Envia um trigger ao ADC12
    ADC12CTL0  |=   ADC12SC;                        // para que ele realize uma medicao
    // P1OUT ^= BIT0;                               // Teste de frequência do timer
}

// Configurar ADC
void adc_config(void){ 
    ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo
    P6SEL |= (BIT0 + BIT1);         // Habilita as entradas em P6.0 P6.1
    ADC12CTL0 = ADC12SHT1_3 +       // Sample and hold time
                ADC12SHT0_3 +       // Sample and hold time
                ADC12ON     +       // Liga o ADC
                ADC12MSC;           // Modo sequencial!
    ADC12CTL1 = ADC12CSTARTADD_0 +  // Inicio da sequencia = canal 0.
                ADC12SHS_0 +        // Sample and holder "manual"
                ADC12SHP +          // Sampling signal = sampling timer
                ADC12DIV_0 +        // Divisor de clock.
                ADC12SSEL_0 +       // Oscilador do ADC
                ADC12CONSEQ_1;      // Sequence of channels
    ADC12CTL2 = ADC12TCOFF +        // Desliga o sensor de temperatura
                ADC12RES_2;         // Resolução = 12 bits

    ADC12IE     =   BIT1;           // Interrup apenas no final

    ADC12MCTL0  =   0;
    ADC12MCTL1  =   1 | ADC12EOS;   // End of Sequence
    
    ADC12CTL0  |=   ADC12ENC;       // Habilita pois a configuracao acabou
 }

// Configurar TA0
void ta0_config(void){ 
    TA0CTL   = TASSEL__ACLK | MC__UP;               // ACLK e modo 1
    TA0CCR0  = FREQ;                                // Ponto de disparo da interrupção
    TA0CCTL0 = CCIE;                                // Habilita interrupcao do timer

    P1DIR |= BIT2;                                  //Sair TA0.1
    P1SEL |= BIT2;
 }


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

// Escrever dado na porta
void i2c_write(char dado){
    UCB0CTL1 |= UCTR    |                           //Mestre TX
                UCTXSTT;                            //Gerar START
    while ( (UCB0IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB0TXBUF = dado;                               //Escrever dado
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);       //Esperar STT=0
    if ( (UCB0IFG & UCNACKIFG) == UCNACKIFG){       //NACK?
        P1OUT |= BIT0;                              //NACK=Sinalizar problema
        while(1);                                   //Travar execu��o
    }
    UCB0CTL1 |= UCTXSTP;                            //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);       //Esperar STOP
}

//////////////////////////////////////////////////////
////// Deste ponto em  diante n�o ser� corrigido /////
//////    Coloque abaixo suas demais fun��es     /////
//////////////////////////////////////////////////////

// Mostra as barras de acordo com o conversor
void blocks() {
    volatile unsigned int x,y;
    if (flag){                          // Verifica se a flag de conversão está ativa
        flag = 0;                       // Zera a flag para esperar a próxima conversão
        lcd_clear();                    // Limpa a tela do lcd
 
        if(medx > 2160){                // If que determina a posição no eixo x da barra
            x = ((4095-medx)*7)/1935;
            lcd_set_cursor(32-x);
            pblcd_char(7);
        }
        else if (medx < 2020){
            x = ((2030-medx)*9)/2030;
            lcd_set_cursor(25-x);
            pblcd_char(7);
        }
        else {
            lcd_set_cursor(25);
            pblcd_char(7);
        }

    }
}

// Configurar I/O
void io_config() {

    // Configura a chave SW2
    P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
    P1REN |= BIT1;              //Habilitar resistor
    P1OUT |= BIT1;              //Habilitar pullup

    // Configura a chave SW6.2
    P6DIR &= ~BIT2;
    P6REN |= BIT2;
    P6OUT |= BIT2;

    // Configura os LEDS
    P1DIR |= BIT0;              //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;             //Led1 apagado

    P4DIR |= BIT7;              //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;             //Led1 apagado
}

// Configurar USCI_B0 como mestre
void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;         //Ressetar USCI_B1
    UCB0CTL0 =  UCMST    |      //Modo Mestre
                UCMODE_3 |      //I2C
                UCSYNC;         //S�ncrono
    //UCB0BRW = BR10K;          //10 kbps
    UCB0BRW = BR100K;           //100 kbps
    UCB0CTL1 = UCSSEL_3;        //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0;       //Fun��es alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}

// //////////////////// Funções de base do zelenovsky ///////////////////////////

// Incializar LCD modo 4 bits
void lcd_inic(void){

    // Preparar I2C para operar
    UCB0CTL1 |= UCTR    |                       //Mestre TX
                UCTXSTT;                        //Gerar START
    while ( (UCB0IFG & UCTXIFG) == 0);          //Esperar TXIFG=1
    UCB0TXBUF = 0;                              //Sa�da PCF = 0;
    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);   //Esperar STT=0
    if ( (UCB0IFG & UCNACKIFG) == UCNACKIFG)    //NACK?
                while(1);

    // Come�ar inicializa��o
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

// Testar o endereço adr para escrita
char i2c_test(char adr){
    UCB0I2CSA = adr;                //Endere�o a ser testado
    UCB0CTL1 |= UCTR;               //Mestre TX --> escravo RX
    UCB0CTL1 |= UCTXSTT;            //Gerar STASRT
    while ( (UCB0IFG & UCTXIFG) == 0);  //TXIFG indica que j� iniciou o START
    UCB0CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);      //Esperar STOP
    if ((UCB0IFG & UCNACKIFG) == 0) return TRUE;   //Chegou ACK
    else                            return FALSE;  //Chegou NACK
}

// Atraso
void atraso(long x){
    volatile long cont=0;
    while(cont++ < x) ;
}

// Testa qual endereço será usado
void lcd_adr(void) {
    char adr;       //Endere�o do PCF_8574

    if      (i2c_test(0x27) == TRUE) adr=0x27;
    else if (i2c_test(0x3F) == TRUE) adr=0x3F;
    else{
        P1OUT |= BIT0;  //Sinalizar problema
        while(TRUE);    //Travar execu��o
    }
    UCB0I2CSA = adr;    //Endere�o a ser usado
}

/////////////////////////////// Funções LCD /////////////////////////////////

// Usa a função I2C write para mandar um nible de uma palavra para o lcd
void lcd_write_nibble(char nib, int inst) {
    char dado;
    dado = ( (nib & 0x0F) << 4);               // Ganrantir que os 4 bits mais significativos são 0
    dado |= 0x08;

    if (!inst)                                  // Verifica se é instrução (RS = 0) ou escrita (RS = 1)
        dado |= 0x01;

    i2c_write(dado);                            // Escereve dado
    i2c_write(dado | 0x04);                     // Ativa enable
    i2c_write(dado);                            // Escreve dado
}

// Usa a função que escreve um nibble no lcd para escrever um caractere
void lcd_write_char(char dado, int inst) {
    char n1, n2;
    n1 = (dado >> 4) & 0x0F;    // Separa o nibble maior
    n2 = dado & 0x0F;           // Separa o nible menor

    lcd_write_nibble(n1, inst);
    lcd_write_nibble(n2, inst);
}

// limpa o display
void lcd_clear() {
    atraso(40);
    lcd_write_char(0x01,1); // Passa a instrução de limpar o display
    atraso(40);
}

// Coloca o ponteiro na posição selecionada
void lcd_set_cursor(int pos) {
    switch (pos){
    case 1: lcd_write_char(0x80,1); break;
    case 2: lcd_write_char(0x81,1); break;
    case 3: lcd_write_char(0x82,1); break;
    case 4: lcd_write_char(0x83,1); break;
    case 5: lcd_write_char(0x84,1); break;
    case 6: lcd_write_char(0x85,1); break;
    case 7: lcd_write_char(0x86,1); break;
    case 8: lcd_write_char(0x87,1); break;
    case 9: lcd_write_char(0x88,1); break;
    case 10: lcd_write_char(0x89,1); break;
    case 11: lcd_write_char(0x8A,1); break;
    case 12: lcd_write_char(0x8B,1); break;
    case 13: lcd_write_char(0x8C,1); break;
    case 14: lcd_write_char(0x8D,1); break;
    case 15: lcd_write_char(0x8E,1); break;
    case 16: lcd_write_char(0x8F,1); break;

    case 17: lcd_write_char(0xC0,1); break;
    case 18: lcd_write_char(0xC1,1); break;
    case 19: lcd_write_char(0xC2,1); break;
    case 20: lcd_write_char(0xC3,1); break;
    case 21: lcd_write_char(0xC4,1); break;
    case 22: lcd_write_char(0xC5,1); break;
    case 23: lcd_write_char(0xC6,1); break;
    case 24: lcd_write_char(0xC7,1); break;
    case 25: lcd_write_char(0xC8,1); break;
    case 26: lcd_write_char(0xC9,1); break;
    case 27: lcd_write_char(0xCA,1); break;
    case 28: lcd_write_char(0xCB,1); break;
    case 29: lcd_write_char(0xCC,1); break;
    case 30: lcd_write_char(0xCD,1); break;
    case 31: lcd_write_char(0xCE,1); break;
    case 32: lcd_write_char(0xCF,1); break;

    default:
        lcd_write_char(0x80,1);
        break;
    }
}

// Coloca o ponteiro no início da linha 2
void lcd_line_2() {
    atraso(40);
    lcd_write_char(0xC0,1); // Usa a instrução de colocar o ponteiro em posição específico
    atraso(40);
}

// Liga e desliga o cursor + blink (Para desligar apenas o cursor 0x0E)
void lcd_cursor(int state) {
    if (state)
    lcd_write_char(0x0F,1); 
    else
        lcd_write_char(0x0C,1);
}

/////////////////////////////// Funções adicionais /////////////////////////////////

// Debouce para precisão da chave
void debounce(int valor){
            volatile int x; //volatile evita optimizador do compilador
                for (x=0; x<valor; x++); //Apenas gasta tempo
            }

// Monitora a chave s3
int mon_s3(void){
        static int ps1=ABERTA;          //Guardar passado de S3
            if ((P6IN&BIT2) == 0){      //Qual estado atual de S3?
                if (ps1==ABERTA){       //Qual o passado de S3?
                    debounce(DBC);
                    ps1=FECHADA;
                    return TRUE;
                    }
                }
            else {
                if (ps1==FECHADA){      //Qual o passado de S3?
                    debounce(DBC);
                    ps1=ABERTA;
                    return FALSE;
                }
            }
    return FALSE;
}