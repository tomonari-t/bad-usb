#define F_CPU 960000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define SBI(B,V) B|= _BV(V)
#define CBI(B,V) B&=~_BV(V)
#define SBIS(B,V) (uint8_t) (B&_BV(V))
#define SBIC(B,V) (uint8_t)!(B&_BV(V))
#define CLK PB3
#define DATA PB4
#define OK 1
#define NG 0

uint8_t clock(void)
{
  CBI(DDRB, CLK);
  if (bit_is_clear(PINB, CLK)) return NG;
  SBI(DDRB, CLK);
  CBI(PORTB, CLK);
  _delay_us(30);
  SBI(PORTB, CLK);
  _delay_us(30);
  return OK;
}

uint8_t send_DATA(uint8_t o)
{
  SBI(DDRB, DATA);
  if(o){SBI(PORTB, DATA);}
  else {CBI(PORTB, DATA);}
  return clock();
}

uint8_t send_ps2(uint8_t data)
{
  uint8_t i, x, parity;
  CBI(DDRB, CLK);
  if (bit_is_clear(PINB, CLK)) return NG;
  CBI(DDRB, DATA);
  if (bit_is_clear(PINB, DATA)) return NG;
  SBI(DDRB, DATA);
  if (!send_DATA(0)) return NG;
  parity = 0;
  for (i = 0; i < 8; i++) {
    x = (data>>i)&1;
    parity += x;
    if (!send_DATA(x)) return NG;
  }
  if (!send_DATA(!(parity&1))) return NG;
  if (!send_DATA(1)) return NG;
  _delay_ms(10);
  return OK;
}

void send_cmd(uint8_t data)
{
  uint16_t j;
  for (j = 0; j < 250; j++) {
    if (send_ps2(data) == OK) break;
  }
}

static const uint8_t key[] PROGMEM = {
  0x1C, 0xF0, 0x1C, 0xF1, // 'a'
  0x32, 0xF0, 0x32, 0xF1, // 'b'
  0x21, 0xF0, 0x21, 0xF1, // 'c'
  0x23, 0xF0, 0x23, 0xF1, // 'd'
  0x24, 0xF0, 0x24, 0xF1, // 'e'
  0x2B, 0xF0, 0x2B, 0xF1, // 'f'
  0x34, 0xF0, 0x34, 0xF1, // 'g'
  0x33, 0xF0, 0x33, 0xF1, // 'h'
  0x29, 0xF0, 0x29, 0xF1, // ' '
  0x5A, 0xF0, 0x5A, 0xF1, // '\n'
  0x00
};

int main(void)
{
  _delay_ms(1500);
  send_cmd(0xAA);
  _delay_ms(2000);
  while (1) {
    uint16_t j;
    for (j = 0; j < sizeof(key); j++) {
      uint8_t x = pgm_read_byte(&key[j]);
      switch (x) {
      case 0xF1: // Wait
        _delay_ms(500);
        continue;
      case 0xFE: // End
        return 1;
      default:
        send_cmd(x);
      }
    }
  }
  return 1;
}
