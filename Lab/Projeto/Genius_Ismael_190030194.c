#include <msp430.h> 

#define TRUE    1
#define FALSE   0

// Valor do CCR0 do timer principal de disparo do conversor AD
#define FREQ   500

// Defines de notas utilizadas
#define N1 261		//DÓ
#define N2 294		//Ré
#define N3 329		//Mi
#define N4 349		//Fá
#define N5 391      //Sol

// Bits para controle do LCD
#define BIT_RS   BIT0
#define BIT_RW   BIT1
#define BIT_E    BIT2
#define BIT_BL   BIT3
#define BR100K  11          //(SMCLK) 1.048.576/11  ~= 100kHz

// Bits para controle da chave
#define ABERTA 1
#define FECHADA 0
#define DBC 1000

void io_config();
void adc_config(void);
void ta0_config(void);
void ta2_config(void);
void USCI_B0_config(void);
void atraso(long x);

void debounce(int valor);
int mon_s3(void);

void i2c_write(char dado);
void lcd_inic(void);
void lcd_aux(char dado);
char i2c_test(char adr);
void lcd_adr();

void lcd_write_nibble(char nib, int inst);
void lcd_write_char(char dado, int inst);
void lcd_clear();
void lcd_set_cursor(int pos);
void lcd_line_2();
void lcd_cursor(int state);
void lcd_write_str(unsigned char *texto);
int strlen(char *s);
void lcd_write_int(unsigned int num, int casas);

void led(char cor);
void nota(int freq, int tempo);
char rand_2b(void);
char make_rand();
void play_all(unsigned char *vt, int qtd);
int play_verify(unsigned char *vt, int play); 

volatile int i, medx, medy, flag=0, contador=0, aux=0;
volatile unsigned int can0, can1;

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Inicializações e configurações
    io_config();
    adc_config();
    ta0_config();
    ta2_config();
    USCI_B0_config();

    lcd_adr();
    i2c_write(0);
    lcd_inic();

    lcd_clear();

    __enable_interrupt();

    volatile char rand;                             // variável que recebe o número aletório a cada rodada
    volatile int jogando, jogada, check, erro;      // Flags de verificação
    volatile int max = 5;                           // Valor total de rodadas 
    volatile unsigned char seq[10];                 // Array que guarda a sequência dos números aleatórios
    
    while(TRUE){                                    // Loop externo que roda toda a sequência do programa
        lcd_clear();

        lcd_write_str("Are you ready?");            // Mensagem incial que espera o jogador iniciar a rodada
        lcd_line_2();
        lcd_write_str("Press the joy.");

        contador=0;                                 // Zera o contador que indica a rodada atual, ou seja, o tamanho que está a sequência

        while (mon_s3() != TRUE){                   // Aguarda o jogador apertar a chave
            nota(0,300);                            // Enquanto aguarda, ocupa o buzzer com uma frequência bem baixa pois ele precisa estar sempre emitindo alguma coisa
        }
        
        

        while(TRUE){                                // Loop interno da partida, toda uma sequência ocorre aqui dentro
        if (flag) {                                 // Verifica se a flag de conversão do joystick está ativa

            lcd_clear();
            rand = make_rand();                     // Recebe o valor aleatório de 1 a 4
            seq[contador] = rand;                   // Guarda esse valor no array de sequência
            contador++;                             // Incrementa o contador para indicar que agora há um novo valor na sequência
            erro=FALSE;                             // Seta a flag de erro como false

            if(contador ==(max+1)){                 // Verifica se o contador atingiu o valor máximo definido
                lcd_write_str("You win!");          // Mensagem de vitória no LCD
                nota(N1, 80);                       // Som de vitória composto por sequência de beeps
                nota(0, 80);
                nota(N1, 80);
                nota(N1, 80);
                nota(N3, 80);
                nota(N1, 80);
                nota(N3, 80);
                nota(N4, 200);
                 __delay_cycles(100000);            //Delay para manter a mensagem por um tempo maior
                break;                              // Sai do loop de partida e vai para o loop geral do programa
            }
                
            lcd_write_str("Wait the end");          // Mensagem que indica que a sequência está sendo executada pelo programa
            lcd_line_2();
            lcd_write_str("of sequence");
            nota(0,1500);                           // Mantem uma nota baixa para ocupar o buzzer e gastar algum tempo

            play_all(seq, contador);                // Função que lê o array e reproduz a sequência de valores do array até o valor do contador
            lcd_clear();                            // Limpa o lcd

            aux = 0;                                // Variável auxiliar que conta de maneira crescente o número de jogadas aceitas até alcançar o contador

            lcd_write_str("Be a genius!");          // Mensagem que indica que o programa está pronto para aceitar uma sequência
            lcd_line_2();
            lcd_write_str("Level 0");
            lcd_write_int(contador,1);              // Indica no LCD o número da rodada atual e a quantidade total de todadas definidas
            lcd_write_str(" de ");
            lcd_write_int(max,2);

            do {                                    // Do wwhile que verifica se a variável auxiliar chegou no contador, se não, aceita o próximo elemento da sequência
                jogando = 1;                        // Flag que indica que o jogador não moveu o joystick ainda
                nota(0, 200);                       // Mantém o buzzer ocupado com uma nota baixa
                while(jogando){                     // While que verifica o input através do joystick pela flag jogando
                    
                    if (medx > 2950){               // If que verifica se o jogador moveu o joystick para a direita
                        jogada = 4;                 // Caso sim, a jogada recebe o valor 4 referente a direita
                        led('G');                   // Liga o led da direita (valor 4)
                        nota(N4, 200);              // Emite o som respectivo
                        jogando = 0;                // Flag jogando agora é zero, pois um input foi recebido
                    }
                    else if (medx < 1100){
                        jogada = 2;
                        led('Y');
                        nota(N2, 200);
                        jogando = 0;
                    }
                    else if (medy > 2950) {
                        jogada = 3;
                        led('R');
                        nota(N3, 200);
                        jogando = 0;
                    }
                    else if (medy < 1100) {
                        jogada = 1;
                        led('B');
                        nota(N1, 200);
                        jogando = 0;
                    }
                    else {                          // Caso nenhum input tenha sido recebido
                        led('Z');                   // Matém-se os leds apagados
                        nota(0, 200);               // Ocupa-se o buzzer com uma nota baixa
                    }
                }
                led('Z');                           // Apaga os leds acesos após a jogada
                check = play_verify(seq, jogada);   // Verifica se a jogada atual é equivalente à sequência na posição indicada pela variável auxiliar
                if(check)                           // Se a jogada estiver correta
                    aux++;                          // Incrementa a variável auxiliar e parte para o próximo input do jogador na sequência
                else{                               // Se a jogada estiver errada
                    erro = TRUE;                    // Ativa a flag de erro, e sai do loop de jogada
                    break;
                }
                    
            }
            while(aux < contador);                  // Condição do Do while que recebe inputs enquanto forem corretor e a variável auxiliar for menor que o contador

            if(erro){                               // No loop de partida, se a flag de erro estiver ativa
                lcd_clear();
                lcd_write_str("You loose.");        // Mensagem de derrota
                lcd_line_2();
                lcd_write_str("Try again later");
                nota(0,1100);                       // Nota baixa e gasto tempo para ler a mensagem
                break;                              // Sai do loop de partida
            }

                
        }
    
    }
}

    return 0;

}

/////////////////////////////////////////////////////////////////////////////////
////////////////////// Rotinas de tratamento de Interrupções ////////////////////
/////////////////////////////////////////////////////////////////////////////////

#pragma vector = ADC12_VECTOR    //54
__interrupt void adc_isr(void){                     // Tratamento da interrupção do AD para estabilizar o sinal do joystick

        can0 += ADC12MEM0;                      // Soma a medicao de X a variavel can0
        can1 += ADC12MEM1;                      // Soma a medicao de Y a variavel can1
        i++;                                    // Incrementa i (contador de conversões)

        if(i == 16){                            // Quando as 8 amostras de cada ja tiverem sido colhidas
                                    
                medx = can0/16;                 // Calcula a media de X
                medy = can1/16;                 // Calcula a media de Y
                flag = 1;                       // Seta a flag para indicar isso
                can0 = 0;                       // Prepara essas variaveis para a proxima soma
                can1 = 0;
                i = 0;                          // Zera o i
                // P4OUT ^= BIT7;
        }
 }

#pragma vector = TIMER0_A0_VECTOR                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                     // Quando o CCR0 chega ao maximo, acontece a interrupcao
{
    ADC12CTL0  &=  ~ADC12SC;                        // Envia um trigger ao ADC12
    ADC12CTL0  |=   ADC12SC;                        // para que ele realize uma medicao
    // P1OUT ^= BIT0;                               // Teste de frequência do timer
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Configurações ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Configurar ADC
void adc_config(void){ 

    volatile unsigned char *pt;
    unsigned char i;

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

    pt=&ADC12MCTL0;

    for (i=0; i<16;  i+=2){
        pt[i+0]=ADC12SREF_0 | ADC12INCH_14; //ADC12MCTL0 at� ADC12MCTL7
        pt[i+1]=ADC12SREF_0 | ADC12INCH_15; //ADC12MCTL8 at� ADC12MCTL15
    }
    pt[15] |= ADC12EOS;

    ADC12MCTL0  =   0;
    ADC12MCTL1  =   1 | ADC12EOS;   // End of Sequence
    
    ADC12CTL0  |=   ADC12ENC;       // Habilita pois a configuracao acabou
 }

// Configurar TA0
void ta0_config(void){ 
    TA0CTL   = TASSEL__ACLK | MC__UP;               // ACLK e modo 1
    TA0CCR0  = FREQ;                                // Ponto de disparo da interrupção
    TA0CCTL0 = CCIE;                                // Habilita interrupcao do timer
 }

// Configurar TA2
void ta2_config(void){
    TA2CTL = TASSEL_2|ID_0|MC_1|TACLR;
    TA2EX0 = TAIDEX_0;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR0 = 2*1048;
    TA2CCR2 = TA2CCR0/2;
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

    P8DIR |= BIT2;              //Led Vermelho = P8.2 = saída
    P8OUT &= ~BIT2;             //Led apagado

    P3DIR |= BIT7;              //Led Azul = P3.7 = saída
    P3OUT &= ~BIT7;             //Led apagado

    P4DIR |= BIT0;              //Led Verde = P4.0 = saída
    P4OUT &= ~BIT0;             //Led apagado
    P4SEL &= ~BIT0;             // Seleciona GPIO

    P4DIR |= BIT3;              //Led Amarelo = P4.3 = saída
    P4OUT &= ~BIT3;             //Led apagado
    P4SEL &= ~BIT3;             // Seleciona GPIO


    P2DIR |= BIT5;	            //Saída PWM (buzzer)
    P2OUT &= ~BIT5;
    P2SEL |= BIT5;
}

// Configurar USCI_B0 como mestre
void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;         //Ressetar USCI_B1
    UCB0CTL0 =  UCMST    |      //Modo Mestre
                UCMODE_3 |      //I2C
                UCSYNC;         //Síncrono
    //UCB0BRW = BR10K;          //10 kbps
    UCB0BRW = BR100K;           //100 kbps
    UCB0CTL1 = UCSSEL_3;        //SMCLK e UCSWRST=0
    P3SEL |= BIT1 | BIT0;       //Funções alternativas
    P3REN |= BIT1 | BIT0;
    P3OUT |= BIT1 | BIT0;
}

// Atraso
void atraso(long x){
    volatile long cont=0;
    while(cont++ < x) ;
}

// Debouce para precisão da chave
void debounce(int valor){
    volatile int x;
        for (x=0; x<valor; x++);        //Apenas gasta tempo
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

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////// LCD inicialização / Funções ////////////////////////
/////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////// Funções LCD ///////////////////////////////////

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

// Parte de uma função que recebe um int e escreve no lcd. Como estava utilizando no programa apenas 2 dígitos, apaguei o código completo que ia até 8
void lcd_write_int(unsigned int num, int casas) {
    volatile int aux, rest, n;

    switch (casas){
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
    default:
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

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Funções espcíficas programa ////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Gerar tom com frequência = "freq" Durante período = tempo mili_seg
void nota(int freq, int tempo){
    volatile int cont=0,comp0,comp2;
    if (freq != 0){
        comp0=TA2CCR0 = 1048576L/freq;
        TA2CCR0 = comp0;				//Prog freq

        comp2 = TA2CCR0/2;
        TA2CCR2 = comp2;				//Carga 50%
        cont = freq*(tempo/1000.);		//Quantidade de períodos
    }
    else{
        P2SEL &= ~BIT5;					//Desligar saída
        comp0=TA2CCR0 = 1048576L/1000;	//Programar 1 kHz
        comp2=TA2CCR2 = TA2CCR0/2;		//Carga  50%
        cont = tempo;					//Quantidade de períodos
    }
    while(cont != 0){
        while( (TA2CCTL0&CCIFG) == 0);	//Usar CCIFG para
        TA2CCTL0 &= ~CCIFG;				//contar quantidade
        cont--;							//de períodos
    }
    P2SEL |= BIT5;
}

// Função que ativa o led na respectiva cor recebida
void led(char cor) {
    switch (cor){
    case 'R':                   // Red
        P8OUT |= BIT2;
        P3OUT &= ~BIT7;
        P4OUT &= ~BIT0;
        P4OUT &= ~BIT3;
        break;
    case 'B':                   // Blue
        P8OUT &= ~BIT2;
        P3OUT |= BIT7;
        P4OUT &= ~BIT0;
        P4OUT &= ~BIT3;
        break;
    case 'G':                   // Green
        P8OUT &= ~BIT2;
        P3OUT &= ~BIT7;
        P4OUT |= BIT0;
        P4OUT &= ~BIT3;
        break;
    case 'Y':                   // Yellow
        P8OUT &= ~BIT2;
        P3OUT &= ~BIT7;
        P4OUT &= ~BIT0;
        P4OUT |= BIT3;
        break;
    default:                    // Apagado
        P8OUT &= ~BIT2;
        P3OUT &= ~BIT7;
        P4OUT &= ~BIT0;
        P4OUT &= ~BIT3;
        break;
    }
}

//Gera aleatorio de 2 bits
char rand_2b(void){
    volatile unsigned int *pt;
    unsigned int x,i;
        pt=&ADC12MEM0;
        ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
    while ( ADC12IFG==0);
            x=0;
                for (i=0; i<16; i++){
                    x+=pt[i]&BIT0;
                }
    return x&3;
}

// Por também estar utilizando o conversor AD para leitura do joystick, não pude utilizar
// a configuração total do gerador com o ADC, o gerador atual só gera um valor de 0 a 2
// então vou gerar dois desses dois bits e somar eles, que aí eu tenho um range de 0 a 4
// (0+0) (0+1) (0+2) (1+1) (1+2) (2+2) - E assim ignorarei o zero tendo um valor de 1 a 4
char make_rand(){
    volatile unsigned char rand1=0, rand2=0,rand;
    
    while(1){
        rand1 |= rand_2b();     // Utiliza a função de gerador aleaório de quase dois bits (de 0 a 2)
        rand2 |= rand_2b();
        rand = rand1 + rand2;   // Soma esses dois valores aleatórios
        if (rand != 0)          // Se a soma for diferente de zero (1 a 4)
            break;              // Sai do loop
    }
    return rand;                // Retorna o valor aleatório tratado
}

// Função que recebe um ponteiro para o vator de sequência e executa a sequência com os leds e beep do buzzer
void play_all(unsigned char *vt, int qtd) {
    volatile int i;
    for(i=0; i<qtd; i++){
        switch (vt[i]){         // Verifica qual é o atual elemento da sequência
        case 1:                 // Se for 1
            led('B');           // Acende o respectivo led da direção 1 (Up)
            nota(N1, 200);      // Executa o som 1
            break;
        case 2:
            led('Y');
            nota(N2, 200);
            break;
        case 3:
            led('R');
            nota(N3, 200);
            break;
        case 4:
            led('G');
            nota(N4, 200);
            break;
        default:
            break;
        }
    }
    led('Z');                   // Nos intervalos entre os elementos, apagar os leds
}

// Função que apenas verifica se a jogada está coerente com o elemento vigente da sequência indicado pela variável global auxiliar
int play_verify(unsigned char *vt, int play) {
    if (vt[aux] == play)
        return TRUE;
    else
        return FALSE;
}
