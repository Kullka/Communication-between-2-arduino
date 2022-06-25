#include <avr/interrupt.h>

volatile int      groupBits[8] = {1, 1, 1, 1, 1, 1, 1, 1};
volatile bool     idle = true;
volatile int      index = 0;
int               pinOut = 2;
char              c = 'a';
int i = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinOut, OUTPUT);
  setBaudRate();
}

void loop() {
  // put your main code here, to run repeatedly:
 if (i<3) {
  uartSend(c);
  i++;
  c++;
  delay(5);
 }
 else {
  c='a';
  i=0;
 }
}

void setBaudRate() {
  cli();  // tat ngat toan cuc
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  TCNT1 = 63869;
  TCCR1B |= (1<<CS10);
  TIMSK1 = (1<<TOIE1);   // overflow interrupt enable
  sei();
}

void uartSend(char c) {
  if (idle) {
    idle = false;
    for (int bits = 7; bits>=0; bits--)
      if (c&(1<<bits))
        groupBits[bits] = 1;
      else
        groupBits[bits] = 0;
    TCNT1 = 63869;
    index = 0;
  }
}

ISR (TIMER1_OVF_vect) {
  TCNT1 = 63869;
  if (!idle) {
    if (index==0) {
      digitalWrite(pinOut, LOW);
    }
    else if (index==9) {
      digitalWrite(pinOut, HIGH);
      idle = true;
    }
    else if (index>0 && index<9) {
      if (groupBits[index-1]) {
        digitalWrite(pinOut, HIGH);
      }
      else {
        digitalWrite(pinOut, LOW); 
      }
    }
    index++;
  }
  else {
    digitalWrite(pinOut, HIGH);
  }
}
