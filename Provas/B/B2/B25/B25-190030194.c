// 1900301984 - Ismael Ithalo Barbosa Pinto
//
// Conex�es I2C
// SCL : P3.0 ===> P4.1
// SDA : P3.1 <==> P4.2

#include <msp430.h> 

#define TRUE  1
#define FALSE 0
#define SEMENTE 5

#define BR10K 50 //Divisor do clock
#define ESCR_WR 0 //Endereçar Escravo para escrever 

#define ABERTA  1        //Estado aberto
#define FECHADA 0        //Estado fechada
#define DBC     1000    //Chute para o debounce

// Fun��es disponibilizadas
// N�o devem ser alteradas
void USCI_B1_config(void);
void USCI_B1_rx(void);
void USCI_B1_tx(void);

// Fun��es a serem escritas
void leds(char z);
void escr_wr(char x);
char escr_busca(void);
void USCI_B0_config(void);
void io_config(void);

char i2c_test(char adr, char modo);
int mon_s1(void);
int mon_s2(void);
void debounce(int valor);

int passado=0; // Variavel que guarda o estado anterior do contador para fazer a maquina de estados que reemove o piscado

int main(void){
    volatile char cont=0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    io_config();
    USCI_B0_config();
    USCI_B1_config();

    while(TRUE){
        if (mon_s1() == TRUE) cont++; //Chave 1?
        if (mon_s2() == TRUE) cont--; //Chave 2?
        leds(cont);
    }

    return 0;
}

//////////////////////////////////////////////////////
////////////////// Funções escritas //////////////////
//////////////////////////////////////////////////////

/* Atualizar os leds em fun��o de z (0, 1, 2 ou 3 )
A piscada estranha comentada na questão, vem do fato de para garantir que o contador esteja zera com as instruções disponíveis,
o que dá pra fazer é ligar os dois leds e depois inverter. Dessa forma, para que seja possível remover o piscado, devemos ter controle
de qual é o estado anterior do contador, e fazer uma máquina de estado simples, que a partir do estado anterior, define qual operação fazer para o atual*/
void leds(char z){
    switch (z&3){               // Compara o valor do contador e faz as devidas operações, o &3 é para impedir valores como -1 e 4
    case 0:
        if(passado==0){}        // Se o estado passado é zero e o atual é zero, nada é feito
        else if(passado==3){    // Se o estado passado é três e o atual é zero, deve-se inverter os dois leds
            escr_wr('1');
            escr_wr('2');
            passado=0;
        }
        else if(passado==1){    // Se o estado passado é 1 e o atual é zero, deve-se inverter o led 2
            escr_wr('2');
            passado=0;
        }
        break;
    case 1:
        if(passado==1){}        // Se o estado passado é 1 e o atual é 1, nada é feito   
        else if(passado==0){    // Se o estado passado é 0 e o atual é 1, deve-se inverter o led 2
            escr_wr('2');
            passado=1;
        }
        else if(passado==2){    // Se o estado passado é 2 e o atual é 1, deve-se inverter o estado dos dois leds
            escr_wr('1');
            escr_wr('2');
            passado=1;
        }
        break;
    case 2:
        if(passado==2){}        // Se o estado passado é 2 e o atual é 2, nada é feito
        else if(passado==3){    // Se o estado passado é 3 e o atual é 2, deve-se inverter o led 2
            escr_wr('2');
            passado=2;
        }
        else if(passado==1){    // Se o estado passado é 1, e o atual é 2, deve-se inverter o estado dos dois leds
            escr_wr('1');
            escr_wr('2');
            passado=2;
        }
        break;
    case 3:
        if(passado==3){}        // Se o estado passado é 3 e o atual é 3, nada é feito
        else if(passado==2){    // Como já existe uma operação para ligar os dois leds, nã importa o passado
            escr_wr('3');
            passado=3;
        }
        else if(passado==0){    // Como já existe uma operação para ligar os dois leds, nã importa o passado
            escr_wr('3');
            passado=3;
        }
        break;
    default:
        break;
    }
}

// Escreve um byte no escravo
void escr_wr(char x){
    volatile char adr; //Endereço de escravo
    adr = escr_busca();

    UCB0I2CSA = adr;    // Passa para o mestre o endereço do escravo

    UCB0CTL1 |= UCTR | UCTXSTT;                 //Gerar START 
    while ( (UCB0IFG & UCTXIFG) == 0);          //Esperar TXIFG=1 (START)
    UCB0TXBUF=x;                                //Coloca o valor a ser transmitido no TXBUF

    while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT);    //Esperar TXSTT=0 (confirmação)

    UCB0CTL1 |= UCTXSTP;                        //Gerar STOP
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP);   //Esperar STOP

    // P4OUT |= BIT7; //Verde indica sucesso    

}

// Retorna o endere�o do escravo
// Logica da funcao foi retirada do ER 9.7
char escr_busca(void){
    volatile char escravo = 0; //endereço do Escravo Receptor que será retornado 
    volatile char adr; //Endereço de escravo

    // Procurar pelo escravo receptor
    for (adr=1; adr<128; adr++){
        if (i2c_test(adr,ESCR_WR) == TRUE){
            escravo=adr;
            if (escravo) {
                break;
            }
        }
    }
    
    return escravo;
}

// Configurar USCI_B0 como mestre
// P3.1 = SDA e P3.0 = SCL
void USCI_B0_config(void){
    UCB0CTL1 = UCSWRST;                     //Resetar USCI_B0
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;   //Modo mestre | Modo I2C | Síncrono
    UCB0BRW =  BR10K;
    UCB0CTL1 = UCSSEL_3;                    //SMCLK e UCSWRST=0

    P3SEL |= BIT0 | BIT1;                   //Função alternativa
    P3REN |= BIT0 | BIT1;                   //Hab resistor
    P3OUT |= BIT0 | BIT1;                   //Pullup

}

// Configurar I/O
void io_config() {

    // Configura a chave SW2
    P1DIR &= ~BIT1;             //S2 = P1.1 = entrada
    P1REN |= BIT1;              //Habilitar resistor
    P1OUT |= BIT1;              //Habilitar pullup

    // Configura a chave SW1
    P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
    P2REN |= BIT1;              //Habilitar resistor
    P2OUT |= BIT1;              //Habilitar pullup

    // Configura os LEDS
    P1DIR |= BIT0;              //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;             //Led1 apagado

    P4DIR |= BIT7;              //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;             //Led1 apagado
}

/////// Função adicionada para complementar ////////
// Testar o endereço adr para escrita ou leitura 
char i2c_test(char adr, char modo){
    UCB0I2CSA = adr;                //Endereço a ser testado

    if (modo==ESCR_WR){             //WR = Escravo receptor
        UCB0CTL1 |= UCTR;           //Mestre TX --> escravo RX 
        UCB0CTL1 |= UCTXSTT;        //Gerar STASRT 
        while ( (UCB0IFG & UCTXIFG) == 0); //TXIFG=1, START iniciado 
    }
    else{                           //RD = Escravo transmissor 
        UCB0CTL1 &= ~UCTR;          //Mestre RX <-- escravo TX 
        UCB0CTL1 |= UCTXNACK;       //NACK ao receber um dado 
        UCB0CTL1 |= UCTXSTT;        //Gerar START
        while ( (UCB0CTL1 & UCTXSTT) == UCTXSTT); //Esperar START 
    }

    UCB0CTL1 |= UCTXSTP;                      //Gerar STOP 
    while ( (UCB0CTL1 & UCTXSTP) == UCTXSTP); //Esperar STOP

    //Teste do ACK
    if ((UCB0IFG & UCNACKIFG) == 0) 
        return TRUE;    //Chegou ACK 
    else 
        return FALSE; //Chegou NACK 
}

// Monitorar S1, retorna TRUE se foi acionada
int mon_s1(void){
    static int ps1=ABERTA;      //Guardar passado de S1
    if ( (P2IN&BIT1) == 0){     //Qual estado atual de S1?
        if (ps1==ABERTA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps1==FECHADA){       //Qual o passado de S1?
            debounce(DBC);
            ps1=ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

// Monitorar S2, retorna TRUE se foi acionada
int mon_s2(void){
    static int ps2=ABERTA;      //Guardar passado de S2
    if ( (P1IN&BIT1) == 0){     //Qual estado atual de S2?
        if (ps2==ABERTA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=FECHADA;
            return TRUE;
        }
    }
    else{
        if (ps2==FECHADA){       //Qual o passado de S2?
            debounce(DBC);
            ps2=ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

// Debounce
void debounce(int valor){
    volatile int x;
    for (x=0; x<valor; x++);
}

//////////////////////////////////////////////////////
//// As fun��es abaixo n�o devem ser alteradas ///////
//////////////////////////////////////////////////////

void USCI_B1_config(void){
    UCB1CTL1 = UCSWRST;         //Resetar USCI_B1
    UCB1CTL0 = UCMODE_3 |       //Modo I2C
               UCSYNC;          //S�ncrono
    UCB1I2COA = 5;             //Endere�o do Escravo
    UCB1CTL1 = 0;               //UCSWRST=0
    UCB1IE = UCTXIE | UCRXIE;   //Hab interrp TX e RX
    P4SEL |= BIT2 | BIT1;       //Fun��o alternativa
    P4REN |= BIT2 | BIT1;       //Hab resistor
    P4OUT |= BIT2 | BIT1;       //Pullup
    __enable_interrupt();       //GIE=1, hab int geral
}

// ISR da USCI_B1
#pragma vector = USCI_B1_VECTOR         //Vetor 45
__interrupt void isr_USCI_B1(void){
    static unsigned int nend=SEMENTE;
    int n;
    n = __even_in_range(UCB1IV,0xC);    //Consultar UCB1IV
    switch(n){
        case 0x0: break;            case 0x2: break;
        case 0x4: break;            case 0x6: break;
        case 0x8: break;
        case 0xA: USCI_B1_rx();  break; //Rotina RX
        case 0xC: USCI_B1_tx();  break; //Rotina TX
    }
    nend = (nend*487) % 127;
    if (nend==0)    nend=1;
    UCB1I2COA = nend;             //Novo Endere�o do Escravo
}

// Rotina para receber um byte e guardar no vetor
void USCI_B1_rx(void){
    char x;
    x=UCB1RXBUF;
    switch(x){
    case '1':   P1OUT ^= BIT0;  break;
    case '2':   P4OUT ^= BIT7;  break;
    case '3':   P1OUT |= BIT0;  P4OUT |= BIT7;  break;
    }
}

// Rotina para transmitir o contador e increment�-lo
void USCI_B1_tx(void){;}

