// 1900301984 - Ismael Ithalo Barbosa Pinto

// Esse numero que aparece na tela são os valores aleatorios. Quando eu pressiono o botão para trocar de canal a interrupção trava o lcd
// Não consegui resolver, mas fiz várias das funções solicitadas, a interrupção do timer e do adc estão lá no começo das funções que estão fora da correção,
// para caso haja alguma duvida do que está acontecendo
// O esqueleto não queria compilar por causa do vetor barras, por isso foi comentado

// Conex�es LCD
// P3.0 = SDA
// P3.1 = SCL

#include <msp430.h> 

#define TRUE    1
#define FALSE   0

#define QTD 800         //Quantidade de n�meros aleat�rios

#define FREQ  20971     //(1.048.576 x 0,02) 

#define BR10K  105      //(SMCLK) 1.048.576/105 ~= 10kHz
#define BR100K  11      //(SMCLK) 1.048.576/11  ~= 100kHz

// Bits para controle do LCD
#define BIT_RS   BIT0
#define BIT_RW   BIT1
#define BIT_E    BIT2
#define BIT_BL   BIT3

#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000

// Vetor para facilitar a cria��o das barras
// char barras[64]= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,   //0
//                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F,   //1
//                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,   //2
//                   0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,   //3
//                   0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //4
//                   0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //5
//                   0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,   //6
//                   0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};  //7
char barras[64];

// Fun��es para criar as barras
void lcd_barras(void);
void pblcd_char(char x);
void pblcd_cmdo(char x);
void i2c_write(char dado);     //<<== atualizar essa fun��o

// Fun��es a serem elaboradas
char rand(void);
void vet_rand(char *vt);
int freq(char x, char *vet);
void barraxy(char x, int freq);
void adc_canal(char canal);
void ta0_config(void);
void adc_config(void);

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
void lcd_write_str(unsigned char *texto);
int adc_canal_valor();

void debounce(int valor);
int mon_s3(void);

volatile char vetor[QTD];   //Vetor para receber os n�meros aleat�rios

volatile int flag = 0, i=0,impar=0,canal=1,rand_2=0;

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    ta0_config();
    adc_config();

    io_config();
    USCI_B0_config();

    lcd_adr();                  // Testa o endereço a ser usado
    i2c_write(0);               //Todos os pinos em 0
    lcd_inic();
    lcd_barras();
    lcd_clear();

    __enable_interrupt();

    while(TRUE){

        if (mon_s3() == TRUE) {     // Chave s3?
            canal++;
            if (canal == 16)
                canal = 0;
            lcd_clear();
            char texto[] = "CANAL";
            lcd_write_str(texto);
            lcd_write_int(canal,0);
            __delay_cycles(1000000);
            adc_canal(canal);
            lcd_clear();
        }

        lcd_write_int(rand_2,0);
        
    }
    return 0;
}

char rand(void){
    volatile unsigned char rand;
    if (flag) {
        flag=0;
        rand = impar;
        impar = 0;

        return rand;
    }
}

void vet_rand(char *vt){
    volatile int j;

    for(j=0; j < QTD; j++){
        vt[j] = rand();
    }

}

int freq(char x, char *vet){
    volatile int j, cont = 0;

    for (j = 0; j < QTD; j++){
        if (vet[j] == x)            //Compara o elemento atual do vetor com o numero dado
            cont++;                 //Incrementa se for igual
    }

    return cont;                    //Retorna o contador
}

void barraxy(char x, int freq){;}

// Verifica qual o canal vigente e configura o buffer
void adc_canal(char canal){
    ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo
    switch (canal){
    case 0: ADC12MCTL0  =   0;
            ADC12MCTL1  =   1 | ADC12EOS;
            break;
    case 1: ADC12MCTL1  =   0;
            ADC12MCTL2  =   1 | ADC12EOS;
            break;
    case 2: ADC12MCTL2  =   0;
            ADC12MCTL3  =   1 | ADC12EOS;
            break;
    case 3: ADC12MCTL3  =   0;
            ADC12MCTL4  =   1 | ADC12EOS;
            break;
    case 4: ADC12MCTL4  =   0;
            ADC12MCTL5  =   1 | ADC12EOS;
            break;
    case 5: ADC12MCTL5  =   0;
            ADC12MCTL6  =   1 | ADC12EOS;
            break;
    case 6: ADC12MCTL6  =   0;
            ADC12MCTL7  =   1 | ADC12EOS;
            break;
    case 7: ADC12MCTL7  =   0;
            ADC12MCTL8  =   1 | ADC12EOS;
            break;
    case 8: ADC12MCTL8  =   0;
            ADC12MCTL9  =   1 | ADC12EOS;
            break;
    case 9: ADC12MCTL9  =   0;
            ADC12MCTL10  =   1 | ADC12EOS;
            break;
    case 10: ADC12MCTL10  =   0;
            ADC12MCTL11  =   1 | ADC12EOS;
            break;
    case 11: ADC12MCTL11  =   0;
            ADC12MCTL12  =   1 | ADC12EOS;
            break;
    case 12: ADC12MCTL12  =   0;
            ADC12MCTL13  =   1 | ADC12EOS;
            break;
    case 13: ADC12MCTL13  =   0;
            ADC12MCTL14  =   1 | ADC12EOS;
            break;
    case 14: ADC12MCTL14  =   0;
            ADC12MCTL15  =   1 | ADC12EOS;
            break;
    case 15: ADC12MCTL15  =   0;
            ADC12MCTL0  =   1 | ADC12EOS;
            break;
    default: break;
    }
    ADC12CTL0  |=   ADC12ENC;       // Habilita pois a configuracao acabou
}

// Configurar TA0
void ta0_config(void){ 
    TA0CTL   = TASSEL_2 | MC__UP;                   // SMCLK e modo 1
    TA0CCR0  = FREQ;                                // Ponto de disparo da interrupção
    TA0CCTL0 = CCIE;                                // Habilita interrupcao do timer

    P1DIR |= BIT2;                                  //Sair TA0.1
    P1SEL |= BIT2;
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
    
    // ADC12MCTL1  =   0;
    // ADC12MCTL2  =   1 | ADC12EOS;   // End of Sequence
    
    ADC12CTL0  |=   ADC12ENC;       // Habilita pois a configuracao acabou
 }

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
////// Daqui ponto em  diante n�o ser� corrigido /////
//////    Coloque abaixo suas demais fun��es     /////
//////////////////////////////////////////////////////

#pragma vector = TIMER0_A0_VECTOR                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                     // Quando o CCR0 chega ao maximo, acontece a interrupcao    
{
    ADC12CTL0  &=  ~ADC12SC;                        // Envia um trigger ao ADC12
    ADC12CTL0  |=   ADC12SC;                        // para que ele realize uma medicao
    // P1OUT ^= BIT0;                                  // Teste de frequência do timer
}

#pragma vector = ADC12_VECTOR    //54
__interrupt void adc_isr(void){ 
        volatile int aux, impar_2;
        aux = adc_canal_valor();                       // Soma a medicao de X a variavel can0
        i++;                                        // Incrementa i (contador de conversões)

        if (aux % 2 == 0){}
        else {
            impar++;
        }

        if(i == 16){                                    // Quando as 8 amostras de cada ja tiverem sido colhidas
            flag = 1;                                   // Seta a flag para indicar isso
            i = 0;                                      // Zera o i
            impar_2 = impar;
            rand_2 = impar_2;
        }

 }

// Verifica qual o canal vigente e pega o valor do buffer
int adc_canal_valor() {
    volatile int buffer;

    switch (canal){
    case 0: buffer = ADC12MEM0; break;
    case 1: buffer = ADC12MEM1; break;
    case 2: buffer = ADC12MEM2; break;
    case 3: buffer = ADC12MEM3; break;
    case 4: buffer = ADC12MEM4; break;
    case 5: buffer = ADC12MEM5; break;
    case 6: buffer = ADC12MEM6; break;
    case 7: buffer = ADC12MEM7; break;
    case 8: buffer = ADC12MEM8; break;
    case 9: buffer = ADC12MEM9; break;
    case 10: buffer = ADC12MEM10; break;
    case 11: buffer = ADC12MEM11; break;
    case 12: buffer = ADC12MEM12; break;
    case 13: buffer = ADC12MEM13; break;
    case 14: buffer = ADC12MEM14; break;
    case 15: buffer = ADC12MEM15; break;
    default: break;
    }

    return buffer; 
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

// Calcula o tamanho de um numero
int dgtlen(int n)
{
    if(n == 0)
        return 1;
    else
    {
        int c;
        for(c = 0; n; n /= 10)
            c++;
        return c;
    }
}

// Função adaptada para retornar no máximo 3 digitos
void lcd_write_int(unsigned int num, int casas) {
    volatile int aux, rest, n;
    n = dgtlen(num);

    switch (n){
    case 1:
        aux = num + 0x30;
        lcd_write_char(aux,0);
        break;
    case 2:
        aux = (num/10) + 0x30;
        rest = (num%10) + 0x30;
        lcd_write_char(aux,0);
        lcd_write_char(rest,0);
        break;
    case 3:
        aux = (num/100) + 0x30;
        rest = (num%100);
        lcd_write_char(aux,0);
        aux = (rest/10) + 0x30;
        rest = rest%10;
        lcd_write_char(aux,0);
        aux = rest + 0x30;
        lcd_write_char(aux,0);
        break;
    case 4:
        aux = (num/1000) + 0x30;
        rest = (num%1000);
        lcd_write_char(aux,0);
        aux = (rest/100) + 0x30;
        rest = (rest%100);
        lcd_write_char(aux,0);
        aux = (rest/10) + 0x30;
        rest = rest%10;
        lcd_write_char(aux,0);
        break;
    case 5:
        aux = (num/10000) + 0x30;
        rest = (num%10000);
        lcd_write_char(aux,0);
        aux = (rest/1000) + 0x30;
        rest = (rest%1000);
        lcd_write_char(aux,0);
        aux = (rest/100) + 0x30;
        rest = (rest%100);
        lcd_write_char(aux,0);
        break;
    case 6:
        aux = (num/100000) + 0x30;
        rest = (num%100000);
        lcd_write_char(aux,0);
        aux = (rest/10000) + 0x30;
        rest = (rest%10000);
        lcd_write_char(aux,0);
        aux = (rest/1000) + 0x30;
        rest = (rest%1000);
        lcd_write_char(aux,0);
        break;
    case 7:
        aux = (num/1000000) + 0x30;
        rest = (num%1000000);
        lcd_write_char(aux,0);
        aux = (rest/100000) + 0x30;
        rest = (rest%100000);
        lcd_write_char(aux,0);
        aux = (rest/10000) + 0x30;
        rest = (rest%10000);
        lcd_write_char(aux,0);
        break;
    default:
        aux = (num/100000000) + 0x30;
        rest = (num%100000000);
        lcd_write_char(aux,0);
        aux = (rest/10000000) + 0x30;
        rest = (rest%10000000);
        lcd_write_char(aux,0);
        aux = (rest/1000000) + 0x30;
        rest = (rest%1000000);
        lcd_write_char(aux,0);
        break;
    }
}

// Calcula o tamanho de uma string
int strlen(char *s)
{
    char *ptr=s;            // Salva o ponteiro pois serão feita operações com ele
    while (*s!='\0')        // Veridica se o ponteiro aponta para o fim do vetor
        s++;                // Incrementa o ponteiro
    return (int) (s-ptr);   // Retorna o fim menos o começo do vetor
}

// Escreve uma string no lcd recebendo seu ponteiro
void lcd_write_str(unsigned char *texto) {
    int qtd = strlen(texto);    // Pega o tamanho da string
    volatile int i;
            for (i = 0; i < qtd; i++){
                if (i==16)          // Verifica se chegou ao fim da primeira linha
                    lcd_line_2();   
                if (i==32){         // Verifica se chegou ao fim da segunda linha
                    atraso(10000);
                    lcd_clear();
                }
                if (i== 48)
                    lcd_line_2();

                lcd_write_char(texto[i],0); // Escreve o caractere
            }
}