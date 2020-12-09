void lcd_write_int_2(unsigned int n) {
    volatile unsigned int tam, aux, rest; 
    tam = dgtlen(n);

    switch (tam){
    case 1:
        lcd_write_char(('0'+ n),0);
        break;
    case 2:
        aux = n/10;
        rest = n%10;
        lcd_write_char(('0'+ aux),0);
        lcd_write_char(('0'+ rest),0);
        break;
    case 3:
        aux = n/100;
        rest = n%100;
        lcd_write_char(('0'+ aux),0);
        aux = rest/10;
        rest = rest%10;
        lcd_write_char(('0'+ aux),0);
        aux = rest;
        lcd_write_char(('0'+ aux),0);
        break;
    case 4:
        aux = n/1000;
        rest = n%1000;
        lcd_write_char(('0'+ aux),0);
        aux = rest/100;
        rest = rest%100;
        lcd_write_char(('0'+ aux),0);
        aux = rest/10;
        rest = rest%10;
        lcd_write_char(('0'+ aux),0);
        // aux = rest;
        // lcd_write_char(('0'+ aux),0);
        break;
    case 5:
        aux = n/10000;
        rest = n%10000;
        lcd_write_char(('0'+ aux),0);
        aux = rest/1000;
        rest = rest%1000;
        lcd_write_char(('0'+ aux),0);
        aux = rest/100;
        rest = rest%100;
        lcd_write_char(('0'+ aux),0);
        // aux = rest/10;
        // rest = rest%10;
        // lcd_write_char(('0'+ aux),0);
        // aux = rest;
        // lcd_write_char(('0'+ aux),0);
        break;
    
    default:
        break;
    }

}