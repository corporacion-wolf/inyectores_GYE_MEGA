static unsigned int C1 = 15625;   // valor inicial para tener una frecuencia de 1Hz al inicio
unsigned int frecuencia_ext = 0;

void setup() 
{
  // put your setup code here, to run once:
  inicio_timer4();
  
  sei();
  Serial.begin(9600);  

  pinMode(13, OUTPUT);

  
}

void loop() 
{
  // put your main code here, to run repeatedly:
    frecuencia_ext = 15625 / C1;
    Serial.println(frecuencia_ext);
    _delay_ms(500);
    
    
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

void inicio_timer4(void)
{
  TCCR4A = 0B00000000; 
  TCCR4B = 0B00000000; //reset control registers
  TCNT4 = 0;              //reset counter register
  TCCR4B |= 0B11000101;   //1024 prescaler, rising edge, noise canceler
  TIMSK4 |= 0B00100001;   //enable input capture & overflow interrupts
}
