// Módulo 3 visto - Ismael Ithalo Barbosa Pinto
// PINAGEM (P3.0 = SDA  / P3.1 = SCL)
// BITS p/ uso LCD (D7  D6  D5  D4  BL  E   RW  RS)

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

#define TRUE 1
#define FALSE 0
#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000

void debounce(int valor);
int mon_s3(void);
void io_config();

void lcd_inic(void);
void lcd_aux(char dado);
void i2c_write(char dado);
char i2c_read(void);
char i2c_test(char adr);
void USCI_B0_config(void);
void atraso(long x);
void lcd_adr();

void lcd_write_nibble(char nib, int inst);
void lcd_write_char(char dado, int inst);
void lcd_clear();
void lcd_line_2();
int strlen(char *s);
int dgtlen(int n);
void lcd_write_str(unsigned char *texto);
void lcd_write_block_l1(int qtd);
void lcd_write_block_l2(int qtd);
void lcd_set_cursor(int pos);
void lcd_write_char_back(char dado, int pos);
void lcd_cursor(int state);
void lcd_write_int(unsigned int num, int casas);
void lcd_write_int_hex(int num);
void display_modo_1(int a, int a_n1_uni, int a_n1_dec, int a_n2);
void display_modo_1_completo(int a0_n1_uni, int a0_n1_dec, int a0_n2, int a1_n1_uni, int a1_n1_dec, int a1_n2);
void display_modo_2_3(int a, int a_n1_uni, int a_n1_dec, int a_n2, int blocks);
void display_modo_4_layout();
void lcd_write_block_back_mod4_l1(int qtd);
void lcd_write_block_back_mod4_l2(int qtd);
void display_modo_4(int up_left, int up_right, int down_left, int down_right);
void lcd_write_block_mod4_l1(int qtd);
void lcd_write_block_mod4_l2(int qtd);

void ADC_config();
void adc_pulso();
void adc_reset();
void adc_conv();

void modo_1();
void modo_2();
void modo_3();
void modo_4();

volatile unsigned long int media_a0=0, media_a1=0;

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    io_config();                //Leds e chave no 6.2
    ADC_config();               // Configura o ADC
    USCI_B0_config();           //Configurar USCI_B0
    lcd_adr();                  // Testa o endereço a ser usado
    i2c_write(0);               //Todos os pinos em 0
    lcd_inic();
    lcd_clear();

    // lcd_cursor(0);

    unsigned char texto[] = "Please, press   the buttom";
    lcd_write_str(texto);
    
    //display_modo_1_completo(2,35,23,3,312,1325);
    //display_modo_2_3(0,2,20,2,11);
    //display_modo_4(7,7,7,7);
    //lcd_set_cursor(4);
    //lcd_write_char(0x0C,1);

    volatile int modo = 1;

    while(TRUE) {

        if (mon_s3() == TRUE) {
            adc_pulso();
            lcd_cursor(0);
            debounce(DBC);
            switch (modo){
            case 1:
                modo_1();
                //display_modo_1_completo(2,35,23,3,312,1325);
                P4OUT &= ~BIT7;
                P1OUT |= BIT0;
                atraso(10000);
                adc_reset();
                modo++;
                break;
            case 2:
                modo_2();
                //display_modo_2_3(0,2,20,2,11);
                P4OUT |= BIT7;
                P1OUT &= ~BIT0;
                atraso(10000);
                adc_reset();
                modo++;
                break;
            case 3:
                modo_3();
                //display_modo_2_3(1,1,350,1111,3);
                P4OUT |= BIT7;
                P1OUT |= BIT0;
                atraso(10000);
                adc_reset();
                modo++;
                break;
            case 4:
                modo_4();
                //display_modo_4(3,8,0,5);
                P4OUT &= ~BIT7;
                P1OUT &= ~BIT0;
                atraso(10000);
                adc_reset();
                modo = 1;
                break;
            default:
                break;
            }

        }

        }

    return 0;
}

//////////////////// Funções de base do zelenovsky ///////////////////////////

// Incializar LCD modo 4 bits
void lcd_inic(void){

    // Preparar I2C para operar
    UCB0CTL1 |= UCTR    |   //Mestre TX
                UCTXSTT;    //Gerar START
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

// Gerar START e STOP para colocar PCF em estado conhecido
void pcf_stt_stp(void){
    int x=0;
    //UCB0I2CSA = PCF_ADR;        //Endere�o Escravo

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
        while(1);           //Travar execu��o
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
        while(1);           //Travar execu��o
    }
    UCB0CTL1 |= UCTXSTP;                //Gerar STOP + NACK
    while ( (UCB0IFG & UCRXIFG) == 0);  //Esperar RX
    dado=UCB0RXBUF;
    while( (UCB0CTL1&UCTXSTP) == UCTXSTP);
    return dado;
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

// Configurar USCI_B0 como mestre
void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;     //Ressetar USCI_B1
    UCB0CTL0 = UCMST    |   //Modo Mestre
            UCMODE_3 |   //I2C
            UCSYNC;      //S�ncrono
    //UCB0BRW = BR10K;      //10 kbps
    UCB0BRW = BR100K;       //100 kbps
    UCB0CTL1 = UCSSEL_3;    //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0;   //Fun��es alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
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
    P4OUT |= BIT7;      //Verde aceso = OK
    UCB0I2CSA = adr;    //Endere�o a ser usado
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

//////////////////////////////////////////////////////////////////////////
//////////////////// Funções do adc ////////// ///////////////////////////
//////////////////////////////////////////////////////////////////////////

// Configuração da pinagem e registradores do ADC
void ADC_config() {
        ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!

        P6SEL |= (BIT0 + BIT1);     // Habilita as entradas em P6.0 P6.1

        ADC12CTL0 = ADC12SHT1_3 +       // Sample and hold time
                    ADC12SHT0_3 +       // Sample and hold time
                    ADC12ON     +       // Liga o ADC
                    ADC12MSC;       // Modo sequencial!
        ADC12CTL1 = ADC12CSTARTADD_0 +  // Inicio da sequencia = canal 0.
                    ADC12SHS_0 +        // Sample and holder "manual"
                    ADC12SHP +          // Sampling signal = sampling timer
                    ADC12DIV_0 +        // Divisor de clock.
                    ADC12SSEL_0 +       // Oscilador do ADC
                    ADC12CONSEQ_1;   // Sequence of channels
        ADC12CTL2 = ADC12TCOFF +        // Desliga o sensor de temperatura
                    ADC12RES_2;         // Resolução = 12 bits

        // Aproveitando os 16 canais do conversor, as 16 coletas serão feitas de uma única vez porém ALTERNADAS
        // ADC12SREF_0 - Vref- = GND; Vref+ = Vcc = 3.3V  - Referências de conversão

        ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;   // Memória 0 = canal 0.
        ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1;   // Memória 1 = canal 1.
        ADC12MCTL2 = ADC12SREF_0 + ADC12INCH_0;   // Memória 2 = canal 0.
        ADC12MCTL3 = ADC12SREF_0 + ADC12INCH_1;   // Memória 3 = canal 1.
        ADC12MCTL4 = ADC12SREF_0 + ADC12INCH_0;   // Memória 4 = canal 0.
        ADC12MCTL5 = ADC12SREF_0 + ADC12INCH_1;   // Memória 5 = canal 1.
        ADC12MCTL6 = ADC12SREF_0 + ADC12INCH_0;   // Memória 6 = canal 0.
        ADC12MCTL7 = ADC12SREF_0 + ADC12INCH_1;   // Memória 7 = canal 1.
        ADC12MCTL8 = ADC12SREF_0 + ADC12INCH_0;   // Memória 8 = canal 0.
        ADC12MCTL9 = ADC12SREF_0 + ADC12INCH_1;   // Memória 9 = canal 1.
        ADC12MCTL10 = ADC12SREF_0 + ADC12INCH_0;   // Memória 10 = canal 0.
        ADC12MCTL11 = ADC12SREF_0 + ADC12INCH_1;   // Memória 11 = canal 1.
        ADC12MCTL12 = ADC12SREF_0 + ADC12INCH_0;   // Memória 12 = canal 0.
        ADC12MCTL13 = ADC12SREF_0 + ADC12INCH_1;   // Memória 13 = canal 1.
        ADC12MCTL14 = ADC12SREF_0 + ADC12INCH_0;   // Memória 14 = canal 0.
        ADC12MCTL15 = ADC12SREF_0 +                // Memória 15 = canal 1.
                    ADC12INCH_1 +
                    ADC12EOS;                    // Fim da sequência de conversão    
                    
        ADC12IFG = 0;

        ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!
}

// Abaixa e levanta o flanco gatilho da conversão
void adc_pulso() {
    // Gera um pulso para iniciar a conversão
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
}

// Desliga e liga a conversão para resetar
void adc_reset() {
    ADC12CTL0 &= ~ADC12ENC;         // Desabilita as conversões antes de configuraro módulo!
    ADC12CTL0 |= ADC12ENC;          // Habilita o ADC para permitir conversões!
}

// Recebe os valores da conversão, calcula a media e coloca em uma variável global
void adc_conv() {
    volatile unsigned int buffer_a0[8],buffer_a1[8];
    volatile unsigned int i;

    // Espera a conversão terminar no segundo canal!
    while(!(ADC12IFG & ADC12IFG1));

    // Guarda o dado convertido
    buffer_a0[0] = ADC12MEM0;     buffer_a1[0] = ADC12MEM1;
    buffer_a0[1] = ADC12MEM2;     buffer_a1[1] = ADC12MEM3;
    buffer_a0[2] = ADC12MEM4;     buffer_a1[2] = ADC12MEM5;
    buffer_a0[3] = ADC12MEM6;     buffer_a1[3] = ADC12MEM7;
    buffer_a0[4] = ADC12MEM8;     buffer_a1[4] = ADC12MEM9;
    buffer_a0[5] = ADC12MEM10;    buffer_a1[5] = ADC12MEM11;
    buffer_a0[6] = ADC12MEM12;    buffer_a1[6] = ADC12MEM13;
    buffer_a0[7] = ADC12MEM14;    buffer_a1[7] = ADC12MEM15;

    // Soma todos os valores das conversões
    for (i=0; i<8; i++){
        media_a0 = media_a0 + buffer_a0[i];
        media_a1 = media_a1 + buffer_a1[i];
    }

    // Faz três rotações para a direita que é o equivalente a dividir por 8
    media_a0 = (media_a0 >> 3);
    media_a1 = (media_a1 >> 3);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// Funções elaboradas do lcd ///////////////////////////
//////////////////////////////////////////////////////////////////////////

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

// Coloca o ponteiro no início da linha 2
void lcd_line_2() {
    atraso(40);
    lcd_write_char(0xC0,1); // Usa a instrução de colocar o ponteiro em posição específico
    atraso(40);
}

// Calcula o tamanho de uma string
int strlen(char *s)
{
    char *ptr=s;            // Salva o ponteiro pois serão feita operações com ele
    while (*s!='\0')        // Veridica se o ponteiro aponta para o fim do vetor
        s++;                // Incrementa o ponteiro
    return (int) (s-ptr);   // Retorna o fim menos o começo do vetor
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

// Escreve o blocos que ocupam um slot todo qtd vezes na linha 1
void lcd_write_block_l1(int qtd){
    lcd_set_cursor(1);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char(0xFF,0); // Escreve o caractere
            }

}

// Escreve o blocos que ocupam um slot todo qtd vezes na linha 2
void lcd_write_block_l2(int qtd){
    lcd_set_cursor(17);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char(0xFF,0); // Escreve o caractere
            }
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

// Usa a função de colocar o cursor em posição específica para escrever para trás
void lcd_write_char_back(char dado, int pos) {
        if (pos-- == 1) //
            lcd_set_cursor(32);
        lcd_set_cursor(pos--);
        lcd_write_char(dado, 0);
        lcd_set_cursor(pos);
}

// Escreve para trás blocos que ocupam um slot todo qtd vezes a partir do meio da linha 1
void lcd_write_block_back_mod4_l1(int qtd){
    lcd_set_cursor(8);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char_back(0xFF,(9-i)); // Escreve o caractere
            }

}

// Escreve para trás blocos que ocupam um slot todo qtd vezes a partir do meio da linha 2
void lcd_write_block_back_mod4_l2(int qtd){
    lcd_set_cursor(24);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char_back(0xFF,(25-i)); // Escreve o caractere
            }
}

// Escreve o blocos que ocupam um slot todo qtd vezes a partir do meio da linha 1
void lcd_write_block_mod4_l1(int qtd){
    lcd_set_cursor(9);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char(0xFF,0); // Escreve o caractere
            }
}

// Escreve o blocos que ocupam um slot todo qtd vezes a partir do meio da linha 2
void lcd_write_block_mod4_l2(int qtd){
    lcd_set_cursor(25);
    volatile int i;
            for (i = 0; i < qtd; i++){
                atraso(50);
                lcd_write_char(0xFF,0); // Escreve o caractere
            }
}

// Liga e desliga o cursor + blink (Para desligar apenas o cursor 0x0E)
void lcd_cursor(int state) {
    if (state)
    lcd_write_char(0x0F,1); 
    else
        lcd_write_char(0x0C,1);
}

// Recebe um valor e o numero de casas decimais desejadas, e epera com divisões sucessivas para reduzir a base enviando um digito por vez
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

// Recebe um valor em decimal e o converte para hexadecimal
void lcd_write_int_hex(int num) {
    volatile int aux, rest, inv[4],x=0,i;
    
    aux = num;
    while( aux != 0) {
        rest = aux%16;
        if (rest < 10)
            inv[x] = rest+48;
        else
            inv[x] = rest+55;
        aux = aux/16; 
        x++;
    }

    for(i=(x-1); i>=0; i--) {
        lcd_write_char(inv[i],0);
    }
}

// Recebe os valores e os coloca no formato da linha do modo 1
void display_modo_1(int a, int a_n1_uni, int a_n1_dec, int a_n2) {
    lcd_write_char('A',0);
    lcd_write_int(a,1);
    unsigned char a_t1[] = ": ";
    lcd_write_str(a_t1);
    lcd_write_int(a_n1_uni,1);
    lcd_write_char(',',0);
    lcd_write_int(a_n1_dec,2);
    unsigned char a_t2[] = "V  ";
    lcd_write_str(a_t2);
    lcd_write_int_hex(a_n2);
}

// Usa o display modo 1 para montar o layout completo desse modo com as duas linhas
void display_modo_1_completo(int a0_n1_uni, int a0_n1_dec, int a0_n2, int a1_n1_uni, int a1_n1_dec, int a1_n2) {
    lcd_clear();
    display_modo_1(0,a0_n1_uni,a0_n1_dec,a0_n2);
    lcd_line_2();
    display_modo_1(1,a1_n1_uni,a1_n1_dec,a1_n2);
}

// Usa a linha do modo 1 e a funçao de escrever blocos para mostrar o modo 2
void display_modo_2_3(int a, int a_n1_uni, int a_n1_dec, int a_n2, int blocks) {
    lcd_clear();
    display_modo_1(a,a_n1_uni,a_n1_dec,a_n2);
    lcd_write_block_l2(blocks);
}

// Estrtura visual básica do modo 4
void display_modo_4_layout() {
    lcd_set_cursor(8);
    lcd_write_char(0x7F,0);
    lcd_write_char(0x7E,0);
    lcd_set_cursor(24);
    lcd_write_char(0x7F,0);
    lcd_write_char(0x7E,0);
}

// utiliza as funções de escrever blocos para frente e para trás e o layout do modo 4 para mostrar informações
void display_modo_4(int up_left, int up_right, int down_left, int down_right) {
    lcd_clear();
    display_modo_4_layout();
    lcd_write_block_back_mod4_l1(up_left);
    lcd_write_block_mod4_l1(up_right);
    lcd_write_block_back_mod4_l2(down_left);
    lcd_write_block_mod4_l2(down_right);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// Funções lcd e adc ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void modo_1() {
    volatile unsigned int voltagem_a0_uni,voltagem_a0_dec,voltagem_a1_uni, voltagem_a1_dec;

    adc_conv();             // Chama a função que calcula as médias

    // (3,3-0)/4095 = 806 micro volts  * media = conversão para volts
    voltagem_a0_uni = (media_a0*806)/1000000;
    voltagem_a0_dec = (media_a0*806)%1000000;
    voltagem_a1_uni = (media_a1*806)/1000000;
    voltagem_a1_dec = (media_a1*806)%1000000;

    display_modo_1_completo(voltagem_a0_uni, voltagem_a1_dec, media_a0, voltagem_a1_uni, voltagem_a1_dec, media_a1);
}

void modo_2() {
    volatile unsigned int voltagem_a0_uni,voltagem_a0_dec;
    volatile float media, blocos;

    adc_conv();             // Chama a função que calcula as médias

    // (3,3-0)/4095 = 806 micro volts  * media = conversão para volts
    voltagem_a0_uni = (media_a0*806)/1000000;
    voltagem_a0_dec = (media_a0*806)%1000000;

    media = (media_a0*806)/1000000;
    blocos = (16*media)/3.3;

    display_modo_2_3(0,voltagem_a0_uni,voltagem_a0_dec, media_a0, blocos);
}

void modo_3() {
    volatile unsigned int voltagem_a1_uni,voltagem_a1_dec;
    volatile float media, blocos;

    adc_conv();             // Chama a função que calcula as médias

    // (3,3-0)/4095 = 806 micro volts  * media = conversão para volts
    voltagem_a1_uni = (media_a1*806)/1000000;
    voltagem_a1_dec = (media_a1*806)%1000000;

    media = (media_a1*806)/1000000;
    blocos = (16*media)/3.3;

    display_modo_2_3(1,voltagem_a1_uni,voltagem_a1_dec, media_a1, blocos);
}

void modo_4() {
    volatile unsigned int voltagem_a0_uni,voltagem_a0_dec,voltagem_a1_uni, voltagem_a1_dec;
    volatile float media_1 = 0,media_2=0, blocos_1_1=0,blocos_1_2=0, blocos_2_1=0, blocos_2_2=0;

    adc_conv();             // Chama a função que calcula as médias

    // (3,3-0)/4095 = 806 micro volts  * media = conversão para volts
    media_1 = (media_a0*806)/1000000;

    if (media_1 > 1.6)
        blocos_1_2 = (8*(media_1-1.6))/1.7;
    else if (media_1 < 1.1)
        blocos_1_1 = (8*(1.1-media_1))/1.1;
    
    if (media_2 > 1.6)
        blocos_2_2 = (8*(media_1-1.6))/1.7;
    else if (media_2 < 1.1)
        blocos_2_1 = (8*(1.1-media_1))/1.1;
        
    display_modo_4(blocos_1_1, blocos_1_2, blocos_2_1, blocos_2_2);

}