// 190030194 - Ismael Ithalo Barbosa Pinto
// Gabarito prova B1, Questao 1

#include <msp430.h>

// Configuaraçoes da leitura da chave
#define TRUE 1
#define FALSE 0
#define ABERTA 1 //Chave aberta
#define FECHADA 0
#define DBC 1000
#define DBC_led 10000

//Configuraçao, monitoriamento e uso da chave e led

int mon_s1(void);
void io_config(void);
void debounce(int valor);

//Funçoes basicas dadas no esqueleto

char rand_2b(void);
void adc_config(void);
void tb0_config(void);

//Funçoes necessarias para calcular os valores pedidos

char rand_8b(void);
void rand_vet(unsigned char *vt, int qtd);
void ord_vet(unsigned char *vt, int qtd);
char media_vet(unsigned char *vt, int qtd);
char freq_nr(unsigned char nr, unsigned char *vt, int qtd);

//Funçao adicional para encontrar a moda

char moda_vet(unsigned char *vt, int qtd);

void main(void)
{
        unsigned char vetor[256];
        volatile char max,max_freq;
        volatile char min,min_freq;
        volatile char moda,moda_freq;
        volatile char media;

        WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

        io_config();
        adc_config();
        tb0_config();


        while(TRUE) {

             if (mon_s1() == TRUE) {     //Chave 1?

                P1OUT &= ~BIT0;                        //Desliga o led vermelho
                P4OUT &= ~BIT7;                        //Desliga o led verde

                rand_vet(vetor, 256);                   //Incializa o vetor com randoms de 8 bits
                ord_vet(vetor, 256);                    //Ordena o vetor

                max = vetor[255];                       //Pega o valor maximo
                min = vetor[0];                         //Pega o valor minimo
                media = media_vet(vetor, 256);          //Faz a media
                moda = moda_vet(vetor, 256);            //Encontra a moda

                max_freq = freq_nr(max, vetor, 256);    //Pega a frequencia do valor max
                min_freq = freq_nr(min, vetor, 256);    //Pega a frequencia do valor minimo
                moda_freq = freq_nr(moda, vetor, 256);  //Pega a frequencia da moda

                if (media > 118 && media < 138){
                    P4OUT |= BIT7;                      //Liga o led verde
                    }

                if (moda_freq < 5){
                    P1OUT |= BIT0;                      //Liga o led vermelho
                    }

                debounce(DBC_led);                            //Manter o led ligado por alguns milesimos a mais
             }

        }

}

////////////////////////////////////////////////////////////////////

//Funçao de atraso
void debounce(int valor){
             volatile int x; //volatile evita optimizador do compilador
                 for (x=0; x<valor; x++); //Apenas gasta tempo
             }

//Configuraçao dos pinos de i/o
void io_config(void){

             P1DIR |= BIT0;              //Led1 = P1.0 = saída
             P1OUT &= ~BIT0;             //Led1 apagado

             P4DIR |= BIT7;              //Led2 = P4.7 = saída
             P4OUT &= ~BIT7;             //Led1 apagado

             P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
             P2REN |= BIT1;              //Habilitar resistor
             P2OUT |= BIT1;              //Habilitar pullup

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

//////////////////////////// Nao alterar ///////////////////////////

//Gera aleatorio de 2 bits
char rand_2b(void){
            volatile unsigned int *pt;
            unsigned int x,i;
                pt=&ADC12MEM0;
                ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
            while ( (ADC12IFG&ADC12IFG15)==0);
                    x=0;
                        for (i=0; i<16; i++){
                         x+=pt[i]&BIT0;
                        }
            return x&3;
}

//Configura o conversor ad
void adc_config(void){
            volatile unsigned char *pt;
            unsigned char i;
            ADC12CTL0  &= ~ADC12ENC;         //Desabilitar para configurar
            ADC12CTL0 = ADC12ON;             //Ligar ADC
            ADC12CTL1 = ADC12CONSEQ_1    |   //Modo sequ�ncia de canais
                        ADC12SHS_3       |   //TB0.1
                        ADC12CSTARTADD_0 |   //Resultado a partir de ADC12MEM0
                        ADC12SSEL_3      |   //ADC12CLK = SMCLK
                        ADC12REFON;
            pt=&ADC12MCTL0;
            for (i=0; i<16;  i+=2){
                pt[i+0]=ADC12SREF_0 | ADC12INCH_14; //ADC12MCTL0 at� ADC12MCTL7
                pt[i+1]=ADC12SREF_0 | ADC12INCH_15; //ADC12MCTL8 at� ADC12MCTL15
            }
            pt[15] |= ADC12EOS;                  //EOS em ADC12MCTL15
            ADC12CTL0 |= ADC12ENC;  //Habilitar ADC12
}

// Configura o timer b0
void tb0_config(void){
            TB0CTL = TBSSEL_2 | MC_1;
            TB0CCTL1 = OUTMOD_6;    //Out = modo 6
            TB0CCR0 = 104;          //10 kHz
            TB0CCR1 = TBCCR0/2;    //Carga 50%
}

////////////////////////////////////////////////////////////////////

//Gera aleatorio de 8 bits com numeros de 2 bits da func rand_2b
char rand_8b(void){
            volatile unsigned char rand_8b;
            volatile int i;

            for (i = 0; i < 4; i++){
                rand_8b = (rand_8b<<2);  //Deslocamento para a esquerda duas vezes
                rand_8b |= rand_2b();   //Concatenaçao com os novos 2 bits random
            }

            return rand_8b;
}

//Gera vetor com numeros aleatorios a patir da func rand_8b
void rand_vet(unsigned char *vt, int qtd){
            volatile int i;
            for (i = 0; i < qtd; i++){
                vt[i] = rand_8b();      //Coloca um numero random de 8 bits na posiçao i
            }
}

// Ordena os elementos do vetor vt
void ord_vet(unsigned char *vt, int qtd){
            volatile int i,j, temp;
            qtd -= 1;                   //Essa operaçao esta fora do for pois estava dando erro
            // Bouble sort
            for (i = qtd; i > 0; i--){
                for (j = 0; j < i; j++){
                    if (vt[j]>vt[j+1]){     //Compara elemento atual com o proximo
                        temp = vt[j];       //Caso maior, realiza a operaçao de troca
                        vt[j] = vt[j+1];    // Valor temporario recebeu atual e atual recebeu proximo
                        vt[j+1] = temp;     //proximo recebe valor temporario
                    }
                }
            }
}

// Retorna a media do vetor vt
char media_vet(unsigned char *vt, int qtd){
    /* O valor maximo eh de 255, sendo que ele pode ocorrer 256 vezes.
    Assim, o valor maximo da soma eh de 65280.
    De acordo com a tabela dada no apendice de C, um unsigned int possui
    valor maximo de 65 535, porem colocarei um long para garantir */
            volatile long unsigned int soma;
            volatile int i;

            for (i = 0; i < qtd; i++){
                soma += vt[i];  //Soma todos os valores do vetor
            }

            return soma/qtd;    //Retorna a media aritmetica simples
}

// Retorna a qtd de vezes que o valor nr aparece no vetor vt
char freq_nr(unsigned char nr, unsigned char *vt, int qtd){
            volatile int i, cont = 0;

            for (i = 0; i < qtd; i++){
                if (vt[i] == nr)    //Compara o elemento atual do vetor com o numero dado
                    cont++;         //Incrementa se for igual
            }

            return cont;            //Retorna o contador
}

// Retorna a moda de um vetor
char moda_vet(unsigned char *vt, int qtd){
        volatile int i, moda, aux1, aux2=0;

        for (i = 0; i < qtd; i++){
            aux1 = freq_nr(vt[i], vt, 256);
            if (aux1>aux2){
                aux2=aux1;
                moda=vt[i];
            }
        }

        return moda;                        //Retorna a moda
}
