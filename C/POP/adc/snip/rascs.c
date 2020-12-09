#include <msp430.h>

//CONFIGURAÇÃO ADC
void ADC_config(void){
    volatile unsigned char *pt;       //ponteiro tipo char
    unsigned char i;
    ADC12CTL0  &= ~ADC12ENC;        //Desabilitar para configurar

        ADC12CTL0 = ADC12SHT0_3 | ADC12SHT1_3 |
                    ADC12ON;            //Ligar ADC

        ADC12CTL1 = ADC12CSTARTADD_0 |  //Comeca em ADC12MEM0
                    ADC12SHS_1       |  // Selecionar TA0.1
                    ADC12DIV_0       |  //Clock ADC Divisor = 1
                    ADC12SSEL_3      |  //Clock ADC = SMCLK
                    ADC12CONSEQ_3;      //Seq. canais com repeticao

        ADC12CTL2 = ADC12TCOFF |        // Desligar sensor temperatura
                    ADC12RES_2;         // Resolução 12-bit

        ADC12MCTL0 = ADC12INCH_0;
        ADC12MCTL1 = ADC12INCH_1;
		ADC12MCTL2 = ADC12INCH_0;
        ADC12MCTL3 = ADC12INCH_1;
		...
		ADC12MCTL14 = ADC12INCH_0;
        ADC12MCTL15 = ADC12EOS | ADC12INCH_1;
		
        ADC12CTL0 |= ADC12ENC;          //Habilitar ADC12

        //PSEL eh optativo, soh reduz consumo de energia
        P6SEL |= BIT0;
        P6SEL |= BIT1;
}

void dma0_config(void){ 	//ADC Seq canais 16 conversões

    DMACTL0 = DMA0TSEL_24; 		//disparo = ADC12IFG
    DMA0CTL = DMAIE        |	//Int do = DMA
			  DMADT_2      |    //Modo Rajada
              DMADSTINCR_3 |    //Destino fixo- o buffer TX n incrementa destino
              DMASRCINCR_3 ;    //Fonte incrementa
    DMA0SA = ADC12MEM0_ADR;            //Fonte = v0[] pois posição 0 ja transmitiu
    //DMA0DA = vetor;     //Destino = vetor[0]
    DMA0DA = &vetor[16];     //Destino = vetor[16]
    DMA0SZ = 16;             //Quantidade
    DMA0CTL |= DMAEN;           //Habilitar
}

// Configurar DMA1
// UCA1RXBUF --> (DMA1) --> v1
void dma1_config(void){

    DMACTL0 |= DMA1TSEL_24;     //ADC12IFG
    DMA1CTL = DMADT_0      |    //Modo Simples
              DMADSTINCR_3 |    //Destino incrementa
              DMASRCINCR_0;     //Fonte fixa
              
    DMA1SA = ADC12MEM0_ADR;     //Fonte = UA1RXBUF
    DMA1DA = vet;               //Destino = vet = &vet[0]
    DMA1SZ = 1000;               //Quantidade
    DMA1CTL |= DMAEN;  //Simples repetido
}

// Gerar barras horizontais
void lcd_barras(void){​​



pblcd_cmdo(0x58); // 0101 1000



pblcd_char(1); //18
pblcd_char(3); //19
pblcd_char(3); //1A
pblcd_char(7); //1B
pblcd_char(7); //1C
pblcd_char(0xF); //1D
pblcd_char(0xF); //1E
pblcd_char(0x1F); //1F



pblcd_char(0x10); //20
pblcd_char(0x18); //21
pblcd_char(0x18); //22
pblcd_char(0x1C); //23
pblcd_char(0x1C); //24
pblcd_char(0x1E); //25
pblcd_char(0x1E); //26
pblcd_char(0x1F); //27



pblcd_cmdo(0x80); //Cursor em 0,0
}​​
''
void pblcd_char(char c){​​
char nib;
nib=c&0xF0;
i2c_write(nib|0x9);
i2c_write(nib|0xD);
i2c_write(nib|0x9);
nib=(c<<4)&0xF0;
i2c_write(nib|0x9);
i2c_write(nib|0xD);
i2c_write(nib|0x9);
}​​



void pblcd_cmdo(char c){​​
char nib;
nib=c&0xF0;
i2c_write(nib|0x8);
i2c_write(nib|0xC);
i2c_write(nib|0x8);
nib=(c<<4)&0xF0;
i2c_write(nib|0x8);
i2c_write(nib|0xC);
i2c_write(nib|0x8);
}​​