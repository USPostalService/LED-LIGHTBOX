/*
2017 - Spencer Firchau - LED Box Code with VU meter

originally adapted from:
2009 - robert:aT:spitzenpfeil_d*t:org - V3_x board test
*/

#define __spi_clock 13  // SCK - hardware SPI
#define __spi_latch 10  //Latch Pin
#define __spi_data 11 //MOSI - hardware SPI
#define __spi_data_in 12  //MISO - hardware SPI (unused)
#define __display_enable 9  //display enable pin
#define __rows 8  //rows of RGB leds
#define __max_row __rows - 1  //sets row vairiable to 0-7
#define __leds_per_row 8  //columns
#define __max_led __leds_per_row - 1  //sets column variable to 0-7
#define __brightness_levels 28  //0...15 above 28 is bad for ISR ( move to timer1, lower irq freq ! ) The higher the brightness the slower the programs run
#define __max_brightness __brightness_levels - 1  //sets brightness to 0-x
#define __fade_delay 4  //for the fade function//static function
#define __TIMER1_MAX 0xFFFF  //16 bit CTR
#define __TIMER1_CNT 0X0000 //32 levels --> 0x0130; 38 --> 0x0157 (flicker)
#define __audio_in A1  //audio input pin, analog
#define __strobe 2  //strobe pin on equalizer chip
#define __reset 3 //reset pin on equalizer
#define __total_spectrum 7  //number of spectrum values on EQ chip
#define __spectrum_levels 6  //used to set spectrum from 0-6
#define __switch 4 //button pin to switch displays
#define __switch_cycles 5//how many programs to run through

#include <avr/interrupt.h>  //library needed for ISR
#include <avr/io.h> //AVR library used for atmega

int spectrum[__total_spectrum]; //array for storing audio spectrum values from analog input

byte switch_counter = 0;  //int for storing which display to be on
int switch_state;  //used for checing whether the button is currently pressed
int switch_reading;  //used for reading the button/switch

byte brightness_red[__leds_per_row][__rows];  // memory for RED LEDs
byte brightness_green[__leds_per_row][__rows];  // memory for GREEN LEDs
byte brightness_blue[__leds_per_row][__rows]; // memory for BLUE LEDs




void setup(void)  //defines pins, output/input
{ 
  randomSeed(55); //starts a random number generator
  pinMode(__spi_clock, OUTPUT);
  pinMode(__spi_latch, OUTPUT);
  pinMode(__spi_data, OUTPUT);
  pinMode(__spi_data_in, INPUT);
  pinMode(__audio_in, INPUT);
  pinMode(__strobe, OUTPUT);
  pinMode(__reset,  OUTPUT);
  pinMode(__display_enable, OUTPUT);
  pinMode(__switch, INPUT);

  digitalWrite(__spi_latch, HIGH); //pulls pins high as a start state, for stability
  digitalWrite(__spi_data, HIGH);
  digitalWrite(__spi_clock, HIGH);
  setup_hardware_spi(); //initiallizes serial transfer funtion
  delay(20);  //gives time for serial function to initiallize
  set_matrix_rgb(0, 0, 0);  //set the display to BLACK
  setup_timer1_ovf(); //enable the framebuffer display
  Serial.begin(9600);
}




void loop(void) //calls functions//sets display mode
{ 
  //smile_blink(320,5,50);
  //demo();
  //demo_2();
  //brightness_red[rws,lds] = vls;
  //random_leds();
   //colors();
   
}

void read_button(void) //this function controls the switch, the big red button switches display state
{
    switch_reading = digitalRead(__switch);  //read the button

    if(switch_reading != switch_state)  //if current button reading does not equal switch_state then update switch state to current reading
    {
      switch_state = switch_reading;
      if (switch_state == HIGH)  //if the reading is pressed then update the counter
      {
        switch_counter++;
        set_matrix_rgb(0, 0, 0);  //clears values when switching to new program
        if(switch_counter == __switch_cycles)
        {
          switch_counter = 0;  //resets the counter back to zero after maximum presses
          set_matrix_rgb(0, 0, 0);  //clears values when switching to new program
        }
      }
    }
}

  



//Analogue Display Functions

void random_leds_VU(void)  //randomly lights leds a random hue
{
  set_matrix_rgb(0, 0, 0);
  for (int ctr = 0; ctr <= __spectrum_levels; ctr ++)
  {
    int bar_height = 0;
    bar_height = spectrum [ctr];
    for (int ctr2 = 0; ctr2 <= bar_height; ctr2++)
    {
    set_led_hue(ctr, ((ctr2)), (int)(random(360)));
    }
  }
}

void simple_VU_blue(void)
{
  set_matrix_rgb(0, 0, 0);
  int bar_height = 0; //variable for storing bar height
  for (int ctr = 0; ctr <= __spectrum_levels; ctr++)
  {
    bar_height = spectrum[ctr];
    for (int ctr2 = 0; ctr2 <= bar_height; ctr2++)
    {
      set_led_blue(ctr, ctr2, 255);
    }
  }
}

void simple_VU_red(void)
{
  set_matrix_rgb(0, 0, 0);
  int bar_height = 0; //variable for storing bar height
  for (int ctr = 0; ctr <= __spectrum_levels; ctr++)
  {
    bar_height = spectrum[ctr];
    for (int ctr2 = 0; ctr2 <= bar_height; ctr2++)
    {
      set_led_red(ctr, ctr2, 255);
    }
  }
}

void simple_VU_RGB(void)
{
  set_matrix_rgb(0, 0, 0);
  int bar_height = 0;
  for (int ctr = 0; ctr <= __spectrum_levels; ctr++)
  {
    bar_height = spectrum[ctr];
    for (int ctr2 = 0; ctr2 <= bar_height; ctr2++)
    {
      if(0 <= 0 && ctr2 <= 3)
      {
        set_led_hue(ctr, ctr2, 100); 
      }
      else if(4 <= 4 && ctr2 <=5)
      {
        set_led_hue(ctr, ctr2, 25);
      }
      else 
      {
        set_led_hue(ctr, ctr2, 0);
      }
    }
  }
}


//Non-analogue Display Functions

void random_leds(void)  //randomly lights leds a random hue
{
  set_led_hue((byte)(random(__rows)), (byte)(random(__leds_per_row)), (int)(random(360)));
}




void smile_on(int hue)  //smily with open eyes
{
  set_matrix_rgb(0,0,0);
  set_row_byte_hue(0,B00000000,hue);
  set_row_byte_hue(1,B01100110,hue);
  set_row_byte_hue(2,B01100110,hue);
  set_row_byte_hue(3,B00000000,hue);
  set_row_byte_hue(4,B00011000,hue);
  set_row_byte_hue(5,B10011001,hue);
  set_row_byte_hue(6,B01000010,hue);
  set_row_byte_hue(7,B00111100,hue);
}




void smile_off(int hue) //smily with closed eyes
{
  set_row_byte_hue(0,B00000000,hue);
  set_row_byte_hue(1,B00000000,hue);
  set_row_byte_hue(2,B01100110,hue);
  set_row_byte_hue(3,B00000000,hue);
  set_row_byte_hue(4,B00011000,hue);
  set_row_byte_hue(5,B10011001,hue);
  set_row_byte_hue(6,B01000010,hue);
  set_row_byte_hue(7,B00111100,hue);
}




void smile_blink(int hue, byte times, int pause)  //blink a smily 
{
  byte ctr;
 
 for(ctr = 0; ctr < times; ctr++) 
 {
   delay(pause);
   smile_on(hue);
   delay(pause);
   smile_off(hue);
   delay(pause);
   smile_on(hue);
 }
}




void fader(void)  //fade the matrix form BLACK to WHITE and back
{
  byte ctr1;
  byte row;
  byte led;

  for(ctr1 = 0; ctr1 <= __max_brightness; ctr1++)
  {
    for(row = 0; row <= __max_row; row++)
    {
      for(led = 0; led <= __max_led; led++)
      {
        set_led_rgb(row, led, ctr1, ctr1, ctr1);
      }
    }
    delay(__fade_delay);
  }
  
  for(ctr1 = __max_brightness; (ctr1 >= 0) & (ctr1 != 255); ctr1--)
  {
    for(row = 0; row <= __max_row; row++)
    {
      for(led = 0; led <= __max_led; led++)
      {
        set_led_rgb(row, led, ctr1, ctr1, ctr1);
      }
    }
    delay(__fade_delay);
  }
}




void fader_hue(void)  //cycle the color of the whole matrix
{
  int ctr1;
  
  for(ctr1 = 0; ctr1 < 360; ctr1=ctr1+3)
  {
    set_matrix_hue(ctr1);
    delay(__fade_delay);
  }
}




void colors(void) //some diagonal color pattern
{
  int ctr1;
  int ctr2;
  byte row;
  byte led;
  byte tmp1;
  byte tmp2;
  byte tmp2_limit = 16;
  
  for(row = 0; row <= __max_row; row++)
  {
    for(led = 0; led <= __max_led; led++)
    {
      tmp1 = (led+1)*(row+1)/2;
      
      if(tmp1 <= tmp2_limit)
      {
        tmp2 = tmp1;
      }
      else
      {
        tmp2 = tmp2_limit;
      }
      set_led_rgb(row, led, (led+1)*(row+1)/2, tmp2_limit-tmp2, 0);
    }
  }
}




void demo(void)
{
  int ctr;
  
  for(ctr=0; ctr < 200; ctr++)
  { 
    random_leds();
  }
  
  smile_blink(200, 4, 100);
  delay(100);
}




void demo_2(void)
{
byte counter1;
byte counter2;

for (counter1 = 0; counter1 <= 7; counter1++)
{
  for (counter2 = 0; counter2 <= 7; counter2++)
  {
    set_led_rgb(counter1,counter2,255,0,0);
    delay(10);
    set_led_rgb(counter1,counter2,0,255,0);
    delay(10);
    set_led_rgb(counter1,counter2,0,0,255);
    delay(10);
    set_led_rgb(counter1,counter2,255,255,255);
    delay(10);
  }
}
set_matrix_rgb(0,0,0);
}




//Basic functions to set the LEDs

void set_led_red(byte row, byte led, byte red)  //function to store and LED as red
{
  brightness_red[row][led] = red;
}




void set_led_green(byte row, byte led, byte green)  //function to store LED as green
{
  brightness_green[row][led] = green;
}




void set_led_blue(byte row, byte led, byte blue)  //function to store LED as blue
{
  brightness_blue[row][led] = blue;
}




void set_led_rgb(byte row, byte led, byte red, byte green, byte blue) //sets one leds rgb values at once
{
  set_led_red(row, led, red);
  set_led_green(row, led, green);
  set_led_blue(row, led, blue);
}




void set_matrix_rgb(byte red, byte green, byte blue)  //sets the whole matrix one color
{
  byte ctr1;
  byte ctr2;
  
  for(ctr2 = 0; ctr2 <= __max_row; ctr2++)
  {
    for(ctr1 = 0; ctr1 <= __max_led; ctr1++)
    {
      set_led_rgb(ctr2, ctr1, red, green, blue);
    }
  }
}




void set_row_rgb(byte row, byte red, byte green, byte blue) //sets a row one color
{
  byte ctr1;
  
  for(ctr1 = 0; ctr1 <= __max_led; ctr1++)
  {
    set_led_rgb(row, ctr1, red, green, blue);
  }
}




void set_column_rgb(byte column, byte red, byte green, byte blue) //sets one column one color
{
  byte ctr1;
  
  for(ctr1 = 0; ctr1 <= __max_row; ctr1++)
  {
    set_led_rgb(ctr1, column, red, green, blue);
  }
}




void set_row_hue(byte row, int hue) //sets one row one hue
{
  byte ctr1;
  
  for(ctr1 = 0; ctr1 <= __max_led; ctr1++)
  {
    set_led_hue(row, ctr1, hue);
  }
}




void set_column_hue(byte column, int hue) //sets one column one hue
{
  byte ctr1;
  
  for(ctr1 = 0; ctr1 <= __max_row; ctr1++)
  {
    set_led_hue(ctr1, column, hue);
  }
}




void set_matrix_hue(int hue)  //sets the matrix one hue
{
  byte ctr1;
  byte ctr2;
  
  for(ctr2 = 0; ctr2 <= __max_row; ctr2++)
  {
    for(ctr1 = 0; ctr1 <= __max_led; ctr1++)
    {
      set_led_hue(ctr2, ctr1, hue);
    }
  }
}




void set_led_hue(byte row, byte led, int hue) //function called HSV used to calculate hue
{
  //see wikipeda: HSV
  float S=100.0, V=100.0, s=S/100.0, v=V/100.0 , h_i, f, p, q, t, R, G, B;
    
  hue = hue%360;
  h_i = hue/60;
  f = (float)(hue)/60.0 - h_i;
  p = v*(1-s);
  q = v*(1-s*f);
  t = v*(1-s*(1-f));
    
  if(h_i == 0)
  {
    R = v;
    G = t;
    B = p;
  }
  else if(h_i == 1)
  {
    R = q;
    G = v;
    B = p;
  }
  else if(h_i == 2)
  { 
    R = p;
    G = v;
    B = t;
  }
  else if(h_i == 3)
  {
    R = p;
    G = q;
    B = v;
  }
  else if(h_i == 4)
  {
    R = t;
    G = p;
    B = v;
  }
  else
  {
    R = v;
    G = p;
    B = q;
  }
  set_led_rgb(row, led, byte(R*(float)(__max_brightness)), byte(G*(float)(__max_brightness)), byte(B*(float)(__max_brightness)));
}




void set_row_byte_hue(byte row, byte data_byte, int hue)  //sets a row a hue based on a row byte, used for manually drawing patterns
{
  byte led;
  
  for(led = 0; led <= __max_led; led++)
  {
    if((data_byte >> led) & (B00000001))
    {
      set_led_hue(row, led, hue);
    }
    else
    {
      set_led_rgb(row, led, 0, 0, 0);
    }
  }
}





//Functions dealing with hardware specific jobs/settings

void setup_hardware_spi(void)
{
  byte clr;
  // spi prescaler: 
  // SPI2X SPR1 SPR0
  //   0     0     0    fosc/4
  //   0     0     1    fosc/16
  //   0     1     0    fosc/64
  //   0     1     1    fosc/128
  //   1     0     0    fosc/2
  //   1     0     1    fosc/8
  //   1     1     0    fosc/32
  //   1     1     1    fosc/64
  SPCR |= ((1<<SPE) | (1<<MSTR)); //enable SPI as master
  //SPCR |= ( (1<<SPR1) );  //set prescaler bits
  SPCR &= ~ ( (1<<SPR1) | (1<<SPR0) );  //clear prescaler bits
  clr=SPSR; //clear SPI status reg
  clr=SPDR; //clear SPI data reg
  SPSR |= (1<<SPI2X); //set prescaler bits
  //SPSR &= ~(1<<SPI2X);  //clear prescaler bits
}




void setup_timer1_ovf(void) //sets timer variables to control how timer operates (ie what speed)
{
  // Arduino runs at 16 Mhz...
  // Timer1 (16bit) Settings:
  // prescaler (frequency divider) values:   CS12    CS11   CS10
  //                                           0       0      0    stopped
  //                                           0       0      1      /1  
  //                                           0       1      0      /8  
  //                                           0       1      1      /64
  //                                           1       0      0      /256 
  //                                           1       0      1      /1024
  //                                           1       1      0      external clock on T1 pin, falling edge
  //                                           1       1      1      external clock on T1 pin, rising edge
  //
  TCCR1B &= ~((1<<CS11));
  TCCR1B |= ((1<<CS12) | (1<<CS10));
  //normal mode
  TCCR1B &= ~((1<<WGM13) | (1<<WGM12));
  TCCR1A &= ~((1<<WGM11) | (1<<WGM10));
  //Timer1 Overflow Interrupt Enable
  TIMSK1 |= (1<<TOIE1);
  TCNT1 = __TIMER1_MAX - __TIMER1_CNT;
  // enable all interrupts
  sei();  //switches interrupts on
}




ISR(TIMER1_OVF_vect)  //Framebuffer interrupt routine/ what happens here, happens on in the timer
{
  TCNT1 = __TIMER1_MAX - __TIMER1_CNT;

  read_button();
  
  digitalWrite(__reset, HIGH);
  digitalWrite(__reset, LOW);

  for (int ctr = 0; ctr <= __spectrum_levels; ctr++)
  {
    digitalWrite(__strobe, LOW);
    delayMicroseconds(35);  //to allow output to settle
    spectrum[ctr] = analogRead(__audio_in);
    spectrum[ctr] = map(spectrum[ctr], 290, 1023, 0, 7);
    digitalWrite(__strobe, HIGH);

  }

  
  if (switch_counter == 0)
  {
    random_leds_VU();
  }
  else if (switch_counter == 1)
  {
    colors();
  }
  else if (switch_counter == 2)
  {
    random_leds();
  }
  else if (switch_counter == 3)
  {
    smile_on(255);
  }
  else if (switch_counter == 4)
  {
    simple_VU_RGB();
  }


  
  
  byte cycle;
  
  digitalWrite(__display_enable,LOW); //enable display inside ISR
  
  for(cycle = 0; cycle <= __max_brightness; cycle++) //cycle # of times until max brightness, more times the brighter
  {
    byte led = B00000000; //clears row
    byte row = B00000000; //row: current source. on when (1)
    byte red = B00000000; //current sinker, on when (0)
    byte green = B00000000; //current sinker, on when (0)
    byte blue = B00000000;  // current sinker, on when (0)
    byte ground = B00000000;
    
    for(row = 0; row <= __max_row; row++) //these functions cycle through the whole matrix and check to see if an led should be on during a single fram
    {
      for(led = 0; led <= __max_led; led++)
      { 
       if(cycle < brightness_red[row][led])
       {
          red |= (1<<led);
          ground |= ~(1<<row);
       }
       else
       {
          red &= ~(1<<led);
          ground &= ~(1<<(row));
       }
          
       if(cycle <  brightness_green[row][led])
       {
          green |= (1<<led);
          ground |= ~(1<<row);
       }
       else
       {
          green &= ~(1<<led);
          ground &= ~(1<<(row));
       }
       
       if(cycle < brightness_blue[row][led])
       {
          blue |= (1<<led);
          ground |= ~(1<<row);
       }
       else
       { 
          blue &= ~(1<<led);
          ground &= ~(1<<(row));
       }
      }
       
      digitalWrite(__spi_latch,LOW);  //pulls latch for shift registers so they are ready to accept data
      spi_transfer(ground); //transfers data starting with ground because it's last in the daisy chain
      spi_transfer(blue);
      spi_transfer(green);
      spi_transfer(red);
      digitalWrite(__spi_latch,HIGH); //pulls latch closed so they shift registers are not accepting data
    }
  }
  digitalWrite(__display_enable,HIGH);  //disable display outside ISR
}




byte spi_transfer(byte data)  //this function is used to transfer the data serially
{
  SPDR = data;  //Start the transmission
  while (!(SPSR & (1<<SPIF))) //Wait the end of the transmission
  {
  };
  return SPDR;  //return the received byte. (we don't need that here)
}
