#include <math.h>

/* Declaración de variables*/
unsigned int peak = 6;

unsigned int cont = 0;      // crea una variable numerica para el contador para el canal a
unsigned int cont_b = 0;    // crea una variable numerica para el contador para el canal b 

unsigned int cont1 = 0;     // CONTADOR 1 DEL NUMERO DE RPMS DADAS PARA EL STROKEN
unsigned int cont2 = 0;     // CONTADOR 2 DEL NUMERO DE RPMS DADAS PARA EL STROKEN

String tipo = "L";              // tipo LATAM por defecto define peak en 6
String numero = "A";            // ambos canales activos por defecto
String modo = "M";              // modo automatico por defecto
unsigned int on_off = 1;        // maquina on por defecto
unsigned int dutty1 = 12;        // pulsos de hold 1
unsigned int dutty2 = 18;        // pulsos de hold 2
unsigned int freq1 = 300;     // antes conocida como remanente
unsigned int stroken = 0;     // variable para determinar una cantidad fija de pulsos con Electrovalvulas
unsigned int measure = 0;     // varaible para medir cantidad fija de pulsos
    
unsigned int temp = 0;        // variable auxiliar que intercambia si es modo manual o automatico en la interrupcion 
unsigned int desfase = 0;     // Variable para el desfase del canal 2  -> para probar 10 equivale a un desfase de 500us = 0.5ms (wrt al pulso inicial)
unsigned int desfase_aux = 0; // Variable para el desfase del canal 2  -> unicamente para lectura del UART

unsigned long desf_calc = 0;  // variable auxiliar de calculos por overflow de desfase

unsigned int frecuencia_aux = 0;
unsigned int frecuencia_int = 0;
unsigned int RPM_int = 0;

unsigned int frecuencia_ext = 0;
unsigned int RPM_ext = 0;
String cadena = "";

String buffer_uart;
char delimitador = 'k';       // caracter delimitador del mensaje
String temp_str;
int temp_int = 0;

unsigned int aux_led = 0;     // contador auxiliar para poder tooglear el led y que sea visible
unsigned int aux_led2 = 0;     // contador auxiliar para poder tooglear el led y que sea visible

static unsigned int C1 = 15625;   // valor inicial para tener una frecuencia de 1Hz al inicio
static unsigned int cont_freq = 0;        // valor para calcular la frecuencia interna 

float valorx;

void setup() 
{
  cli();                // apaga las interrupciones generales stop interrupts

  inicio_puertos();     // INICIALIZACION DE PUERTOS GPIO
  inicio_timer0();      // INICIALIZACION TIMER 0 EN MODO CTC COMO FSM CADA 50us.
  inicio_external_interrupts();   // INICIALIZACION INTERRUPCIONES EXTERNAS
  inicio_timer4();        // INICIALIZACION TIMER 4 EN MODO CAPTURA PARA CONTAR RPM
  
  sei();                // HABILITACION I EN SREG - INTERRUPCIONES GLOBALES ENABLE

  Serial.begin(9600);       // init serial 9600 bauds
  Serial.setTimeout(10000); // time out serial 10 segundos
}

void loop() 
{
  if(Serial.available())
  {
    buffer_uart = Serial.readStringUntil('k');
    descomponer_trama();    
    sincronizar_canales();
  }

  /* Operaciones */
  // calculo la frecuencia en base a C1 que viene del timer 4 - modo automatico
  frecuencia_ext = 15625 / C1;
  RPM_ext = frecuencia_ext*60;  

  // calculo de la frecuencia que viene de la interfaz en modo manual
  frecuencia_int = (unsigned int) ((20000 / frecuencia_aux));  
  RPM_int = (unsigned int) (frecuencia_int * 60);
  
  valorx = random(0,99);
  valorx /= 100;

  /*Condiciones de la trama*/ 
  analizar_tipo_actuador();   // tipo de actuador
  analizar_modo();        // analizar modo manual o automatico
  analizar_on_off();      // on u off de la maquina peak and hold

  // conversion de frecuencia enviada desde PC de 0 a 360 en el desfase adecuado variando desde 0 a frecuencia 
  if (modo == "M")
  {
    desf_calc = (unsigned long) desfase_aux;
    desf_calc *= freq1;
    desf_calc /= 360;
  }
  if (modo == "A")
  {
 //   desf_calc = (unsigned long) desfase_aux * C1 * 32 / (9000);       // ecuacion de conversion de C1 de 0 a 15625 al desfase de 200 a 20000
    desf_calc = (unsigned long) desfase_aux;
    desf_calc *= C1;
    desf_calc *= 32;
    desf_calc /= 9000;
  }

  desfase = (unsigned int) desf_calc;
           
  Serial.println(cadena);
  _delay_ms(100);
}

void inicio_puertos(void)
{
  DDRA = 0B01101011;          // Se declara el los pines 23, 25, 27 y 28 como salidas CANAL A (1 = output / 0 = input) 
  PORTA = 0B10010100;         // Se define es estado del pin encendido o apagado CANAL A. 
    
  DDRB = 0B00000000;          // declaro el puerto B "PORTB" como entradas "INPUTS" 
  PORTB = 0B00000001;         // declaro el pin 0 "PIN B.0" como entrada "INPUT PULL-UP ==> PIN 8" 
  
  DDRG = 0B00000101;          // DECLARACION DE PINES DE ELECTROVALVULAS 1 Y 2
  PORTG = 0B00000000;         // LOW LEVEL DEFAULT PARA ELECTROVALVULAS 1 Y 2

  DDRJ = 0B00000011;          // DECLARACION DE PINES DE RELES DE PROTECCION FUENTE 80V Y 24V
  PORTJ = 0B00000000;         // LOW LEVEL DEFAULT PARA RELES DE PROTECCION 
  
  DDRC = 0B11010110;        // Se declara el los pines 31, 33, 35 y 36 como salidas CANAL B (1 = output / 0 = input) 
  PORTC = 0B00101001;       // Se define es estado del pin encendido o apagado CANAL B.

  pinMode(13, OUTPUT);    // PIN 13 SALIDA AUXILIAR

  pinMode(21,INPUT);      // PIN 21 ENTRADA INT0
  digitalWrite(21, HIGH);   // entrada en alta impedancia 
}

void inicio_timer0(void)
{
  TCCR0A = 0B00000010;      // Registro del control del Timer/Contador "CTC MODE", bit7, bit5:4 operación normal, bit3:2 siempre cero,   
  OCR0A = 99;               // CLCK FSM = 50us
  TCNT0 = 0;                // RST TIM
  TIMSK0 = 0B00000010;      // INT COMPA
  
  TCCR0B = 0B00000010;      // CLKTIM = CLKSYS / 8 & START TIM
}

void inicio_timer4(void)
{
  TCCR4A = 0; 
  TCCR4B = 0; //reset control registers
  TCNT4 = 0;              //reset counter register
  TCCR4B |= 0b11000101;   //1024 prescaler, rising edge, noise canceler
  TIMSK4 |= 0b00100001;   //enable input capture & overflow interrupts
}

void inicio_external_interrupts(void)
{
   EICRA = 0B11000011;       // CAMBIAR 2 ULTIMOS BITS A 11 PARA RISING EDGE INT
   EIMSK = 0B00000000;       // DESHABILITAR INT0 POR DEFECTO - SE HABILITA DESPUES CON SENAL ON OFF 
}

void analizar_tipo_actuador(void)
{
  if(tipo == "U")         // actuador tipo u requiere peak para corriente en 8
  {
    peak = 8;                 
  }
  else                    // actuador tipo no u requiere peak por defecto para corriente en 6
  {
    peak = 6;
  }
}

void analizar_modo(void) 
{
  if (modo == "M")      // MODO MANUAL
  {
    temp = 0x01;
    cadena =  (String(RPM_int) + ',' + String(frecuencia_int) + ',' + String(desfase_aux) + ',' + String(stroken) + ',' + String(C1) + ',' + String(desfase));
    digitalWrite(13, (bool) temp);    // imagen de la senal temp 
  }
  
  if (modo == "A")      // MODO AUTOMATICO
  {
    temp = 0x00;
    cadena =  (String(RPM_ext) + ',' + String(frecuencia_ext) + ',' + String(desfase_aux) + ',' + String(stroken) + ',' + String(C1) + ',' + String(desfase));
    digitalWrite(13, (bool) temp);    // imagen de la senal temp
  }
}

void analizar_on_off(void)
{
  if (on_off == 0) 
  { 
    PORTJ = 0x00;                 // APAGAR AMBOS RELES PARA PROTEGER LOS MOSFETS 
    TIMSK0 = 0B00000000;          // APAGAR LA INTERRUPCION DEL TIMER0 - DISABLE COMPA
    EIMSK = 0B00000000;           // APAGAR LA INTERRUPCION INT0 PARA AHORRAR PROCESSING
    PORTA &= 0B10010100;          // CLEAR ALL 
    PORTC &= 0B00101001;          // CLEAR ALL 
  }
  
  if(on_off == 1)  
  {
    PORTJ = 0B00000011;           // ENCENDER LOS DOS RELES PARA TRABAJAR CON LOS MOSFETS
    TIMSK0 = 0B00000010;          // HABILITAR LA INTERRUPCION DEL TIMER0 - ENABLE COMPA
    EIMSK = 0B00000001;           // ENABLE INT0 DE PEAK AND HOLD
  }
}

void descomponer_trama(void)
{
    /* TRAMA CONSTRUC => 
      1 BYTE -> TIPO DE ACTUADOR - CHAR
      1 BYTE -> NUMERO DE ACTUADORES - CHAR
      1 BYTE -> MODO DE FUNCIONAMIENTO - CHAR

      1 BYTE -> START O STOP - INT
      3 BYTES -> DUTTY 1 - INT
      3 BYTES -> DUTTY 2 - INT

      5 BYTES -> FRECUENCIA - INT
      3 BYTES -> STROKEN - INT
      1 BYTES -> MEASURE - INT   
    */
  
  /*Tipo de actuador*/ 
    temp_str = buffer_uart.substring(0,1);
    tipo = temp_str;

    /*Numero de actuadores*/
    temp_str = buffer_uart.substring(1,2);
    numero = temp_str;
  
    /*Modo de funcionamiento*/
    temp_str = buffer_uart.substring(2,3);
    modo = temp_str;
    
    /*Start/Stop*/
    temp_str = buffer_uart.substring(3,4);
    temp_int = temp_str.toInt();
    on_off = (unsigned int) temp_int;
    
    /*Dutty 1*/
    temp_str = buffer_uart.substring(4,7);
    temp_int = temp_str.toInt();
    dutty1 = (unsigned int) temp_int;

    /*Dutty 2*/
    temp_str = buffer_uart.substring(7,10);
    temp_int = temp_str.toInt();
    dutty2 = (unsigned int) temp_int;
    
    /*frecuencia*/
    temp_str = buffer_uart.substring(10,15);
    temp_int = temp_str.toInt();
    freq1 = (unsigned int) temp_int;

    /*stroken*/
    temp_str = buffer_uart.substring(15,18);
    temp_int = temp_str.toInt();
    stroken = (unsigned int) temp_int;

    /*desfase*/
    temp_str = buffer_uart.substring(18,21);
    temp_int = temp_str.toInt();
    desfase_aux = (unsigned int) temp_int;

    /* measure */   
    temp_str = buffer_uart.substring(21,22);
    temp_int = temp_str.toInt();
    measure = (unsigned int) temp_int;
}

//======================================================================

void sincronizar_canales(void)        // FUNCION PARA SINCRONIZAR AMBAS SENALES IGUALA CONTADORES
{
  cont_b = cont;
}

void lanzar_stroken()
{
  if(cont2 <= stroken && measure == 1)            // SE DISPARA LA FUNCION STROKEN CON LA SENAL MEASURE DEL PUERTO SERIAL
  {
    PORTG = 0B00000101;             // ENCENDER ELECTROVALVULAS MIENTRAS DURACION DE STROKEN
    cont2++;                        // incrementar contador de stroken - se incrementa una vez por ciclo ya que esta funcion es llamada una sola vez por ciclo en linea 290

    // aqui se deberia empezar la medicion de flujo DE DIESEL utilizando el timer 3 o 5
  }
  if (cont2 > stroken)              // una vez alcanzado stroken se apagan las EV
  {
    PORTG = 0B00000000;             // APAGAR ELECTROVALVULAS
    cont2 = stroken ;            // DEJAR AL CONTADOR FUERA DEL RANGO STROKEN para volver a apagar las EV - es decir volver a esta condicion o a la siguiente

    // aqui se deberia finalizar la medicion de flujo DE DIESEL utilizando el timer 3 o 5 y se deberia pasar el valor a la RAM para se impreso posteriormente
  }
  
  if(cont2 >= stroken && measure == 0)
  {
    cont2 = 0;                      // CON LA SENAL MEASURE = 0 VOLVEMOS A LA CONDICION INICIAL PARA PODER VOLVER A LANZAR UNA NUEVA SECUENCIA 
    PORTG = 0B00000000;             // APAGAR ELECTROVALVULAS
  }
}

ISR(TIMER4_CAPT_vect)
{
  C1 = ICR4;          // LEEMOS EL VALOR CAPTURADO EN EL MOMENTO DEL FLANCO
  TCNT4 = 0;          // ENCERAMOS DE NUEVO EL CONTADOR DEL TIMER 4
}

ISR(TIMER4_OVF_vect)
{
  C1 = 65000;         // overflow significa que la frecuencia es muy baja entonces C1 muy alto para que frecuencia sea 0 en el calculo
}

ISR (TIMER0_COMPA_vect)
{

   if ((cont == 0))
    {
      if (numero != "T")
      {
        lanzar_stroken();
        frecuencia_aux = cont_freq;
        cont_freq = 0;
      }
    }

    if ((cont_b == 0))
    {
      if (numero == "T")
      {
        lanzar_stroken();
        frecuencia_aux = cont_freq;
        cont_freq = 0;
      }
    }
 
  /*========= Canal A ========*/
  
    if (cont == 0)
    {
      PORTA |= 0B01000001;      // init cycle HIGH LEVEL testigo inicio ciclo
    }
  
  if(numero != "T")     // verificamos que el canal A venga habilitado con la variable numero de actuadores - INICIO REAL DE SENALES
  {     
    if (cont < 2)
      PORTA &= 0B11010101;      // A & B & C LOW LEVEL      100 us LOW
  
    else if (cont < peak)  
      PORTA |= 0B00100010;      // A & C HIGH LEVEL
      
    else if (cont < peak+6)                                                                             //******************** no deberia ser peak + 2? o mete 24V constantes por 300us
      PORTA = 0B10111101;      // A LOW & B HIGH & C HIGH and keep cycle indicator & keep pull ups
  
    else if (cont < (dutty1 + peak + 6))
      PORTA ^= 0B00001000;      // GENERACION PWM 50%
    
    else if (cont < freq1)
      PORTA &= 0B11010100;      // CLEAR ALL keep cycle - FIN DEL CICLO
  
    else
    {                                                             // aqui se debe corregir ya aque al cambiar contador se dana para la siguiente verificacion
      PORTA &= 0B11010100;      // CLEAR ALL y fin de ciclo 
      cont = 0XFFFF;
    }

    if (aux_led > 1000 )              // periodo 1000 es de 50ms - se puede cambiar este limite si se desea tooglear el led mas rapido o mas lento 
    {
      PORTA ^= 0B01000000;            // TOGLEAMOS LED INDICADOR DE CICLO A EN FUNCIONAMIENTO
      aux_led = 0;
    }
  }

  if ((temp == 0) && (cont >= (dutty1 + peak + 6)))       // si modo automatio y contador acabo el ciclo  PARA AGREGAR DESFASE EN A SE DEBERIA SUMAR EL DESFASE EN A A LA CONDICION 2
      cont = 20000;                                       // Dejar el contador fuera del rango pero que no vuelva a cero para hacerlo con la INT0 = PARA DESFASE A SE DEBE SUMAR EL DESFASE MAXIMO AL 300     
      
  cont++;
  aux_led++;                    // aumentamos contador de led cada 50us para los leds indicadores

/*========= Canal B ========*/
     if (cont_b == 0)
      PORTC |= 10000010;      // init cycle HIGH LEVEL

  if(numero != "M")           // verificamos que el canal B venga habilitado con la variable numero de actuadores
  {    

  // DESFASE UNICAMENTE SI AMBOS CANALES ESTAN ACTIVADOS Y EL DESFASE SE CREA EN EL CANAL B
      if (desfase < freq1 - dutty2 - peak - 6)
      {    
        if (cont_b < 2  + desfase)
          PORTC &= 0B10101011;      // A & B & C LOW LEVEL
      
        else if (cont_b < peak  + desfase)  
          PORTC |= 0B01000100;      // A & C HIGH LEVEL
      
        else if (cont_b < peak + 6  + desfase)
          PORTC = 0B01111011;      // A LOW & B HIGH & C HIGH and keep cycle indicator & keep pull ups
      
        else if (cont_b < (dutty2 + peak + 6 + desfase))
          PORTC ^= 0B00010000; 
        
        else if (cont_b < freq1)
          PORTC &= 0B10101001;      // CLEAR ALL keep cycle
      
        else
        {
          PORTC &= 0B00101001;      // CLEAR ALL - fin de ciclo 
          cont_b = 0XFFFF;
        }
      }

      else                                        // se debe trabajar en esta parte cuando el desfase se acerca a la frecuencia no se debe perder la onda 
      {
        cont_b = cont - 1 + (freq1 - desfase);    // se adelanta al contador b el desfase necesario
          
        if (cont_b >= freq1)                      // se hace ciclear al contador b con la misma frecuencia del sistema freq1
          cont_b = cont_b - freq1;
        
        if (cont_b < 2)
          PORTC &= 0B10101011;      // A & B & C LOW LEVEL
      
        else if (cont_b < peak)  
          PORTC |= 0B01000100;      // A & C HIGH LEVEL
      
        else if (cont_b < peak + 6)
          PORTC = 0B01111011;      // A LOW & B HIGH & C HIGH and keep cycle indicator & keep pull ups
      
        else if (cont_b < (dutty2 + peak + 6))
          PORTC ^= 0B00010000; 
        
        else if (cont_b < freq1)
          PORTC &= 0B10101001;      // CLEAR ALL keep cycle
      
        else
        {
          PORTC &= 0B00101001;      // CLEAR ALL - fin de ciclo 
          cont_b = 0xFFFF;
        }
      }

      if (aux_led2 > 1000 )             // periodo 1000 es de 50ms - se puede cambiar este limite si se desea tooglear el led mas rapido o mas lento 
      {
        PORTC ^= 0B10000000;            // TOGLEAMOS LED INDICADOR DE CICLO A EN FUNCIONAMIENTO
        aux_led2 = 0;
      }
  }
  if ((temp == 0) && (cont_b >= (dutty2 + peak + 6 + desfase)))    // aqui toca aumentar mas desfase para no borrar el canal b analizar mejor ya que desfase si puede ser mayor a 300 creo que tocaria ponerle en algo mayior a 20.000 o tener contador A contrador B 
      cont_b = 20000;                                              // Dejar el contador fuera del rango pero que no vuelva a cero para hacerlo con la INT0      
      
  cont_b++;
  cont_freq++;                        // incremento el contador de frecuencia para determinar la frecuencia interna 
  aux_led2++;
}

ISR (INT0_vect)
{
  if ((temp == 0) && (cont >= (dutty1 + peak + 6)))
    cont = 0;

  if ((temp == 0) && (cont_b >= (dutty2 + peak + 6)))
    cont_b = 0;
}
