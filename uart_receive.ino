#include <avr/interrupt.h>

volatile int      groupBits[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
volatile bool     idle = true;
volatile unsigned index = 0;
int               pinIn = 2;
int               pinOut = 4;
volatile bool     input = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinIn, INPUT);
  pinMode(pinOut, OUTPUT);
  setBaudRate();
  attachInterrupt(0, uartRead, FALLING);
}

void setBaudRate() {
  cli();  // tat ngat toan cuc
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  TCNT1 = 65510;
  TCCR1B |= (1<<CS11) | (1<<CS10);
  TIMSK1 = (1<<TOIE1);   // overflow interrupt enable
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void uartRead() {
  if (idle) {
    index = 0;
    idle = false;
    TCNT1 = 65515; 
  }
}

ISR (TIMER1_OVF_vect) {
  TCNT1 = 65510;
  if (!idle) {
    input = digitalRead(pinIn);
    if (input) {
      groupBits[index] = 1;
      digitalWrite(pinOut, HIGH);
    }
    else {
      groupBits[index] = 0;
      digitalWrite(pinOut, LOW);
    }
    if (index==10) {
      idle = true;
    }
    index++;
  }
}
