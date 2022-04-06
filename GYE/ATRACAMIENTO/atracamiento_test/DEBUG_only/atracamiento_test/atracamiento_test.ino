#define medidas_por_ciclo 200         // cada ciclo es 100us -> 100 medidas es un intervalo de 10ms

int conversion = 0;

uint32_t ui32_temp = 0;

int calibracion_offset = 0;

int medidas[medidas_por_ciclo + 5] = {};

int i = 0;
unsigned char while_out = 0;
unsigned char flag = 0;

uint8_t cont_pass = 0;             // contador auxiliar para pasar las medidas del vector temporal a los definitivos
unsigned char n = 0;                // contador imprimir medidas 

void setup() 
{
  pinMode(5, OUTPUT);         // salida pwm
  pinMode(13, OUTPUT);

  timer3_init();
  Serial.begin(57600);
  
  sei();

  adc0_init();

  Serial.println("Reposo");

  _delay_ms(1000);

  Serial.println("Calibrando");

  for (uint8_t cont_calib = 0; cont_calib < 25; cont_calib++)       // 25 mediciones de offset en promedio 
  {
    ui32_temp += (uint32_t) adc_conv();
  }
  calibracion_offset = ui32_temp / 25;
  Serial.print("Offset: ");
  Serial.println(calibracion_offset);
}

void loop() 
{
//*************************** 2 V ***********************************//
  OCR3A = 67;              // PWM = 2V in the coil
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)        // while in while making n = 100 measurments in timer interrupt 
  {
    Serial.print(i);
    if (i >= medidas_por_ciclo)                // 100 medidas equivale a 10 ms -> region lineal del actuador
    {   
      while_out = 1;
    }
  }
  flag = 0;               // disable measurement storage
  
/*
  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 3 V ***********************************/
/*
  OCR3A = 100;              // 

//*************************** 4 V ***********************************/
/*
  OCR3A = 133;              // 
  
//*************************** 6 V ***********************************/
/*
  OCR3A = 200;              //
    
  //*************************** 8.0 V ***********************************/
/*
  OCR3A = 267;              //
*/

  parar_timer();

  Serial.println("MEDIDAS OBTENIDAS RAW value");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.println(medidas[n]);
    //Serial.print(",");
  }
  Serial.println("");

  _delay_ms(1000);                  // descanso de la bobina con el mosfet apagado y loop back again
}

void timer3_init(void)
{
  TCCR3A = 0b00000000;
  TCCR3B = 0b00010001;    // PWM fase y frecuencia corregida CLK_TIM = CLK_SYS / 1

  ICR3 = 800;             // 10KHZ

  OCR3A = 0;              // PWM = 0%

  TIMSK3 = 0b00000001;    // OVF INT enabled
}

void arrancar_timer (void)
{
  TCNT3 = 0;              // Come back to 0 the timer counter to avoid PWM parasitic pulses
  TCCR3A = 0b10000000;    // Start pin PWM
}

void parar_timer (void)
{
  OCR3A = 0;              // Off PWM
  TCCR3A = 0b00000000;    // Stop pin PWM 
  PORTE &= 0b11110111;    // PIN OFF 
}

ISR (TIMER3_OVF_vect)
{
  PORTB |= 0b10000000;        // pin 13 indicator on 
  conversion = adc_conv();    // adc conversion when pin 13 is on
  PORTB &= 0b01111111;        // pin 13 indicator off

  if (flag == 1)
  {
    medidas[i] = conversion;
    i++;
  }
}

void adc0_init(void)
{
  ADMUX = 0b00000000;
  ADCSRB &= 0b11110111;

  ADCSRA = 0b10000000;
}

int adc_conv(void)
{
  int conv  = 0;
  
  ADCSRA |= 0b01000000;
  while((ADCSRA & 0b01000000) == 0b01000000);

  conv = ADCL;
  conv += (ADCH << 8);

  return conv;
}
