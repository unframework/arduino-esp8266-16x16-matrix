/*
Legacy pin definitions

#ifdef ESP8266

// ESP8266 Feather pinout (top row starting from corner)
#define LEDARRAY_D 4
#define LEDARRAY_C 5
#define LEDARRAY_B 2
#define LEDARRAY_A 16
#define LEDARRAY_G 0
#define LEDARRAY_DI 15
#define LEDARRAY_CLK 13
#define LEDARRAY_LAT 12

#else

// Arduino pinout (top row starting from corner after TX/RX)
#define LEDARRAY_D 2
#define LEDARRAY_C 3
#define LEDARRAY_B 4
#define LEDARRAY_A 5
#define LEDARRAY_G 6
#define LEDARRAY_DI 7
#define LEDARRAY_CLK 8
#define LEDARRAY_LAT 9

#endif
*/

// Arduino pins 2-7
#define PORTD_D (1 << 2)
#define PORTD_C (1 << 3)
#define PORTD_B   (1 << 4)
#define PORTD_A   (1 << 5)
#define PORTD_G   (1 << 6)
#define PORTD_DI   (1 << 7)

// Arduino pins 8-9
#define PORTB_CLK (1 << 0)
#define PORTB_LAT  (1 << 1)

#define SET(port, pinset) (port |= (pinset))
#define CLR(port, pinset) (port &= (~(pinset)))
#define SET_MASK(port, mask, pinset) (port = (port & (~(mask))) | (pinset))
#define NOP() __asm__("nop\n\t")

// Y-line = 0 is on the side with data connectors
// when it is at the top, X-pixels are fed into shift registers from right to left
int pix[] = {
  0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
  0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
  1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,
  1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,
  1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
  1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,
  0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,
  0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,
  0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,
  0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,
  0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,
  0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,
  0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,
  0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,
  0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void setup() {
  // set pin mode to output
  SET(DDRD, (
    PORTD_D |
    PORTD_C |
    PORTD_B |
    PORTD_A |
    PORTD_G |
    PORTD_DI
  ));

  SET(DDRB, (
    PORTB_CLK |
    PORTB_LAT
  ));

  // reset the X-data latch and clock
  CLR(PORTB, PORTB_LAT);
  CLR(PORTB, PORTB_CLK);

  // turn off display
  SET(PORTD, PORTD_G);
}

void loop() {
  int *pixPtr = pix;

  for (int i = 0; i < 16; i++) {
    // send data and latch it
    for (int dot = 0; dot < 32; dot++) {
      SET_MASK(PORTD, PORTD_DI, *pixPtr ? 0 : PORTD_DI);
      NOP();

      SET(PORTB, PORTB_CLK);
      NOP();
      CLR(PORTB, PORTB_CLK);
      NOP();

      pixPtr += 1;
    }

    SET(PORTB, PORTB_LAT);
    NOP();
    CLR(PORTB, PORTB_LAT);

    // select Y-line
    uint8_t yLinePins = ((i) & 1) ? PORTD_A : 0;
    yLinePins |= ((i >> 1) & 1) ? PORTD_B : 0;
    yLinePins |= ((i >> 2) & 1) ? PORTD_C : 0;
    yLinePins |= ((i >> 3) & 1) ? PORTD_D : 0;
    SET_MASK(PORTD, (PORTD_D | PORTD_C | PORTD_B | PORTD_A), yLinePins);

    // toggle display
    CLR(PORTD, PORTD_G);
    delayMicroseconds(300);
    SET(PORTD, PORTD_G);
  }
}
