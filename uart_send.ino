// Transmitter code

#include <avr/interrupt.h>

String              data = "abcdefahsksa";
int                 lengthOfData = data.length();
volatile int        indexBit = 0;
int                 pinOut = 2;
volatile uint16_t   indexData = 0;
volatile int        groupBit[8] = {1, 1, 1, 1, 1, 1, 1, 1};
volatile bool       start = false;

void setup() {
  
  pinMode(pinOut, OUTPUT);
  digitalWrite(pinOut, HIGH);
  delay(5);
  
  // cho phép ngắt toàn cục 
  for (int i=0; i<8; i++)
    groupBit[i] = (data[0] & (1<<i)) ;
  setUpBaudRate();
}

void setUpBaudRate(){
  cli();  // tat ngat toan cuc
  // reset timer/counter1
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  // Setup timer/counter1
  TCCR1B |= (1<<CS11) | (1<<CS10);
  TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable 
  sei();
  TCNT1 = 65510;
}

void loop() {
  if (start) {
    if (indexBit==0)
      digitalWrite(pinOut, LOW);
    else if (indexBit==9 || indexBit==10)
      digitalWrite(pinOut, HIGH);
    else {
      if (indexData>=lengthOfData)
        digitalWrite(pinOut, LOW);
      else
        if (groupBit[indexBit-1])
          digitalWrite(pinOut, HIGH);
        else
          digitalWrite(pinOut, LOW);
    }
    start = !start;
  }
}

// Trinh phuc vu ngat nay dung de truyen bit theo giao thuc uart 8 bit
ISR (TIMER1_OVF_vect) { // ISR laf keyword, TIMER1_OVF_vect: vector name "Ngat tran tren timer/counter1"
  TCNT1 = 65510;
  start = !start;
  if (indexBit==0) {
    updateDataSent(data[indexData]);
  }
  else if (indexBit==9) {
    digitalWrite(pinOut, HIGH);
  }
  else if (indexBit==10) {
    indexBit = -1;
    if (indexData>=lengthOfData)
        indexData = 0;
    else
        indexData = indexData + 1;
  }
  else {}
  indexBit = indexBit + 1;
}

void updateDataSent(char c) {
  for (int bits = 7; bits > -1; bits--) {
    // Compare bits 7-0 in byte
    if (c & (1 << bits)) {
      groupBit[bits] = 1;
    }
    else {
      groupBit[bits] = 0;
    }
  }
}
