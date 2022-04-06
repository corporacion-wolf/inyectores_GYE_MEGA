#define medidas_por_ciclo 150         // cada ciclo es 100us -> 100 medidas es un intervalo de 10ms -> MAX 8 bit => 255

#define init_atracamiento   25          // threshold en inicio deteccion caida de corriente
#define caidas              10          // numero de caidas necesarias para atracar
#define promedio_inicial_n  10          // cuantos valores se requieren en el promedio inicial


int conversion = 0;

uint32_t ui32_temp = 0;

int calibracion_offset = 0;

int medidas[medidas_por_ciclo + 5] = {};

int medidas3[medidas_por_ciclo + 5] = {};
int medidas4[medidas_por_ciclo + 5] = {};
int medidas5[medidas_por_ciclo + 5] = {};
int medidas6[medidas_por_ciclo + 5] = {};
int medidas7[medidas_por_ciclo + 5] = {};
int medidas8[medidas_por_ciclo + 5] = {};
int medidas9[medidas_por_ciclo + 5] = {};
int medidas10[medidas_por_ciclo + 5] = {};                
int medidas11[medidas_por_ciclo + 5] = {};
int medidas12[medidas_por_ciclo + 5] = {};
int medidas13[medidas_por_ciclo + 5] = {};
int medidas14[medidas_por_ciclo + 5] = {};
int medidas15[medidas_por_ciclo + 5] = {};
int medidas16[medidas_por_ciclo + 5] = {};
int medidas17[medidas_por_ciclo + 5] = {};
int medidas18[medidas_por_ciclo + 5] = {};
int medidas19[medidas_por_ciclo + 5] = {};


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
unsigned char n = 0;               // contador imprimir medidas 


// ************** ATRACAMIENTO *************************//
int max_atracamiento = 0;         // variable para deteccion de caida de corriente
int promedio_last    = 0;         // promedio de las ultimos 10 valores del vector

int atrac_max = 0;                // limite maximo de la gap de atracamiento -> eliminador de picos de ruido
int atrac_min = 0;                // limite minimo de la gap de atracamiento -> aliminador de picos de ruido

int pendiente_cont = 0;           // contador de pendiente en n puntos de caida de corriente

int atracamiento = 0;             // atracamiento detectedo

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

  OCR3A =67;              // PWM = 2V in the coil
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)              // while in while making n = 100 measurments in timer interrupt 
  {
//    Serial.print(i);
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 100 medidas equivale a 10 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas3[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
//*************************** 2.5 V ***********************************//

  OCR3A = 83;              // PWM = 2.5V in the coil
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage
  
  while (while_out == 0)               // while in while making n = 100 measurments in timer interrupt
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas4[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
//*************************** 3 V ***********************************//

  OCR3A = 100;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas5[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }
  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  //*************************** 3.5 V ***********************************//

  OCR3A = 117;              //
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas6[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
//*************************** 4 V ***********************************//

  OCR3A = 133;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas7[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
//*************************** 4.5 V ***********************************//

  OCR3A = 150;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas8[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 5 V ***********************************//

  OCR3A = 167;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas9[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 5.5 V ***********************************//

  OCR3A = 183;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas10[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 6 V ***********************************//

  OCR3A = 200;              //
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas11[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 6.5 V ***********************************//

  OCR3A =217;              // PWM =  in the coil
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas12[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 7.0 V ***********************************//

  OCR3A = 233;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas13[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 7.5 V ***********************************//

  OCR3A = 250;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas14[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
  //*************************** 8.0 V ***********************************//

  OCR3A = 267;              //
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas15[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 8.5 V ***********************************//

  OCR3A = 283;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas16[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
//*************************** 9.0 V ***********************************//

  OCR3A = 300;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas17[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue

//*************************** 9.5 V ***********************************//

  OCR3A = 317;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas18[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  
//*************************** 10.0 V ***********************************//

  OCR3A = 333;              // 
  arrancar_timer();
  i = 0;
  while_out = 0;
  flag = 1;               // enable measurements storage

  while (while_out == 0)
  {
    PORTB |= 0B10000000;        // senal del led Pin 13
    if (i >= medidas_por_ciclo)                // 10 medidas equivale a 1 ms -> region lineal del actuador
      while_out = 1;
  }
  flag = 0;               // disable measurement storage

  for (cont_pass = 0; cont_pass < medidas_por_ciclo; cont_pass++)
  {
    medidas19[cont_pass] = medidas[cont_pass];     // sotcking in other vector the values as medidas is a temporary array 
  }

  parar_timer();
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
  _delay_ms(250);        // tiempo de espera que el inductor se descargue
 
//*************************** END MEASUREMENTS ***********************************//
 
  parar_timer();

  Serial.println("MEDIDAS OBTENIDAS RAW value");
/*
  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas3[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas4[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas5[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas6[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas7[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas8[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas9[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas10[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas11[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas12[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas13[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas14[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas15[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas16[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas17[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas18[n]);
    Serial.print(",");
  }
  Serial.println("");

  for (n = 0; n < medidas_por_ciclo; n++)
  {    
    Serial.print(medidas19[n]);
    Serial.print(",");
  }
  Serial.println("");

  */
    procesar_atracamiento();        // 10 V
    procesar_atracamiento2();        // 9.5 V


  _delay_ms(1000);                  // descanso de la bobina con el mosfet apagado y loop back again
}

void procesar_atracamiento(void)
{
  // encero las variables a utilizar
  promedio_last = 0;
  max_atracamiento = 0;
  pendiente_cont = 0;
  atracamiento = 0;

  // calculadora de dato medio en los ultimos datos segun defines
  for (n = (init_atracamiento - promedio_inicial_n); n < init_atracamiento; n++)
  {    
    promedio_last += medidas19[n];
  }
  
  promedio_last = promedio_last / promedio_inicial_n;
  Serial.println(promedio_last);


  // calculo de atrac_max y minimo y algortimo de deteccion
  for (n = init_atracamiento; n < medidas_por_ciclo; n++)
  { 
    atrac_max = 1.250 * promedio_last;
    atrac_min = 0.25 * promedio_last;

  Serial.println("max min");
  Serial.println(atrac_max);
  Serial.println(atrac_min);
       
    if ( (medidas19[n] > atrac_min) && (medidas19[n] < atrac_max) )
    {
      
      promedio_last = medidas19[n];             // actualizo el valor aceptado
      Serial.println(promedio_last);

      
      // dato valido entonces se debe verificar la pendiente en 10 puntos
      if (medidas19[n] > max_atracamiento)      // si la medida actual es mayor entoncesse actualiza el maximo ya que es un dato valido y es mayor al anterior pendiente creciente
      {
        max_atracamiento = medidas19[n];
        pendiente_cont = 0;                     // si hay un valor actual mayor al maximo detectado anteriormente pues se vuelve a encerar el contador de pendiente ya que asi se fitra el ruido de datos espurios
      }
      else                                      // si es menor no actualizo el valor maximo y se incrementa el contador hasta que alcance el numero de caidas necesarias para deteccion
      {
        pendiente_cont++;                       // incremento contador de caida de pendiente
        if (pendiente_cont >= caidas)
        {
          atracamiento = 100;                   // atracamiento detectado en 10V - decivoltios
          Serial.print("atracamiento detectado en ");
          Serial.println(atracamiento);
        } 
      }
  }
 }
}


void procesar_atracamiento2(void)
{
  // encero las variables a utilizar
  promedio_last = 0;
  max_atracamiento = 0;
  pendiente_cont = 0;
  atracamiento = 0;

  // calculadora de dato medio en los ultimos datos segun defines
  for (n = (init_atracamiento - promedio_inicial_n); n < init_atracamiento; n++)
  {    
    promedio_last += medidas19[n];
  }
  
  promedio_last = promedio_last / promedio_inicial_n;
  Serial.println(promedio_last);


  // calculo de atrac_max y minimo y algortimo de deteccion
  for (n = init_atracamiento; n < medidas_por_ciclo; n++)
  { 
    atrac_max = 1.250 * promedio_last;
    atrac_min = 0.25 * promedio_last;

  Serial.println("max min");
  Serial.println(atrac_max);
  Serial.println(atrac_min);
       
    if ( (medidas18[n] > atrac_min) && (medidas19[n] < atrac_max) )
    {
      
      promedio_last = medidas18[n];             // actualizo el valor aceptado
      Serial.println(promedio_last);

      
      // dato valido entonces se debe verificar la pendiente en 10 puntos
      if (medidas18[n] > max_atracamiento)      // si la medida actual es mayor entoncesse actualiza el maximo ya que es un dato valido y es mayor al anterior pendiente creciente
      {
        max_atracamiento = medidas18[n];
        pendiente_cont = 0;                     // si hay un valor actual mayor al maximo detectado anteriormente pues se vuelve a encerar el contador de pendiente ya que asi se fitra el ruido de datos espurios
      }
      else                                      // si es menor no actualizo el valor maximo y se incrementa el contador hasta que alcance el numero de caidas necesarias para deteccion
      {
        pendiente_cont++;                       // incremento contador de caida de pendiente
        if (pendiente_cont >= caidas)
        {
          atracamiento = 950;                   // atracamiento detectado en 10V - cantiivoltios
          Serial.print("atracamiento detectado en ");
          Serial.println(atracamiento);
        } 
      }
  }
 }
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

ISR (TIMER3_OVF_vect)               // se ejecuta cada periodo de pwm cada 10us pwm 10khz 
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

  ADCSRA = 0b10000000;        // 1 us per conversion

  //  ADCSRA = 0b10000010;    // 4 us
  //  ADCSRA = 0b10000011;    // 8us
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
