int conversion = 0;

uint32_t ui32_temp = 0;

int calibracion_offset = 0;

int medidas[15] = {};

int medidas0[15] = {};                
int medidas1[15] = {};
int medidas2[15] = {};
int medidas3[15] = {};
int medidas4[15] = {};
int medidas5[15] = {};
int medidas6[15] = {};
int medidas7[15] = {};
int medidas8[15] = {};
int medidas9[15] = {};
int medidas10[15] = {};                
int medidas11[15] = {};
int medidas12[15] = {};
int medidas13[15] = {};
int medidas14[15] = {};
int medidas15[15] = {};
int medidas16[15] = {};
int medidas17[15] = {};
int medidas18[15] = {};
int medidas19[15] = {};



int resistencias[50] = {};
int inductancias[50] = {};

float temp_res = 0;
float temp_vol = 0;
float resistencia = 0;
float inductancia = 0;

int i = 0;
unsigned char while_out = 0;
unsigned char flag = 0;

unsigned char cont_ind = 0;

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
  //*************************** 0.5 V ***********************************//
  OCR3A = 17;              // PWM = 2.12% = 0.5V in the coil
  arrancar_timer();       // arrancamos PWM
  flag = 1;               // flag used to notify timer interrupt that it is ready to start saving measurements

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (uint8_t cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas0[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }


/*************REPEAT PROCESS FOR DIFFERENT PWM VOLTAGES ************/
//*************************** 1 V ***********************************//

  OCR3A = 33;              // PWM = 4.16% = 1V in the coil
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas1[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }
  
//*************************** 1.5 V ***********************************//

  OCR3A = 50;              // PWM = 1.5V in the coil
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas2[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 2 V ***********************************//

  OCR3A =67;              // PWM = 2V in the coil
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas3[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 2.5 V ***********************************//

  OCR3A = 83;              // PWM = 2.5V in the coil
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas4[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 3 V ***********************************//

  OCR3A = 100;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas5[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }


  //*************************** 3.5 V ***********************************//

  OCR3A = 117;              //
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas6[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 4 V ***********************************//

  OCR3A = 133;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas7[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }


//*************************** 4.5 V ***********************************//

  OCR3A = 150;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas8[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 5 V ***********************************//

  OCR3A = 167;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas9[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }



//*************************** 5.5 V ***********************************//

  OCR3A = 183;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas10[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }
  
//*************************** 6 V ***********************************//

  OCR3A = 200;              //
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas11[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 6.5 V ***********************************//

  OCR3A =217;              // PWM =  in the coil
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas12[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 7.0 V ***********************************//

  OCR3A = 233;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas13[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 7.5 V ***********************************//

  OCR3A = 250;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas14[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }


  //*************************** 8.0 V ***********************************//

  OCR3A = 267;              //
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas15[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 8.5 V ***********************************//

  OCR3A = 283;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas16[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }


//*************************** 9.0 V ***********************************//

  OCR3A = 300;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas17[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 9.5 V ***********************************//

  OCR3A = 317;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas18[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

//*************************** 10.0 V ***********************************//

  OCR3A = 333;              // 
  i = 0;
  while_out = 0;

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= 10)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }

  for (cont_pass = 0; cont_pass < 11; cont_pass++)
  {
    medidas19[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();

  Serial.println("MEDIDAS OBTENIDAS RAW value");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas0[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas1[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas2[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas3[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas4[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas5[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas6[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas7[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas8[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas9[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas10[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas11[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas12[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas13[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas14[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas15[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas16[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas17[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas18[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas19[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < 10; n++)
  {    
    Serial.print(medidas[n]);
    Serial.print(",");
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

void adc1_init(void)
{
  ADMUX = 0b00000001;
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
