#include <U8x8lib.h>

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);


void setup(void) {
  u8x8.begin();
}

void loop(void) {
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0,0,"Hello World!");
  delay(2000);
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.setInverseFont(1);
  u8x8.drawString(2, 2, "12345");
  u8x8.setInverseFont(0);
  u8x8.drawString(2, 3, "67892");
  delay(2000);
  u8x8.begin();
  u8x8.setFont(u8x8_font_5x8_f);
  u8x8.draw2x2String(0, 5, "mipt");
  delay(2000);
}
