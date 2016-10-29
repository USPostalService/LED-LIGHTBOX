
//Definitions, some of these are used to assign ports, called like a funtion

#define number_of_gray_scale_values 4 //corresponds to 6 gray scale values// =4 because compare value goes from 0-6

#define Latch 2  //pins from 0 to x on each port latch is arduino pin 10 but is pin 2 on PORTB
#define Clock 3 //pin 11 arduino pin 3 on PORTB
#define Data 1 //pin 9 on arduino pin 1 on PORTB

#define Latch_Low   PORTB &= ~(1 << Latch)//Sets that pin to 0
#define Latch_High  PORTB |=  (1 << Latch)//Sets that pin to 1
#define Clock_Low   PORTB &= ~(1 << Clock)//Sets that pin to 0
#define Clock_High  PORTB |=  (1 << Clock)//Sets that pin to 1
#define Data_High   PORTB &= ~(1 << Data) //Sets that pin to 0
#define Data_Low    PORTB |=  (1 << Data) //Sets that pin to 1

/*---------------------------------------------------------------------------------------------------------------------------------------------*/

//Global Variables
unsigned char dispmem[64];//64 positions in the array
static unsigned char *ptr_dispmem;//Pointer to Display memory, used to access postions in display memory array

//Color Look Up tables for the color scales
unsigned char LUT_r[216]; //unsigned char is the same as byte type, why i'm using unsigned char is not fully clear right now, after it works i may try replacing it with byte to see if it works fine.
unsigned char LUT_g[216];
unsigned char LUT_b[216];

unsigned char compare_value = 0; //used to check during the drawing if the loop needs to be incremented or if it has been satified and can stop
unsigned char row = 0; //for going through rows
unsigned char x = 0; //for advancing 0-7

/*---------------------------------------------------------------------------------------------------------------------------------------------*/

void LUT_init(void)//fills color values in a look up table, shades 0 to 5
{
  for (byte i = 0; i < 216; i++)//fills 216 colors total
  {
    LUT_r[i] = (byte)(i/36); //byte division rounds/truncates to lowest whole number 35/36 => 0 and 37/36 => 1 and 73/36 => 2
    LUT_g[i] = (byte)((i%36)/6);//modulus operator spits out remainder so 7 modulus 36 is 36 goes into 7 zero times, the left over is 7, so 7/6 => 1 as it is truncated
    LUT_b[i] = i%6;
  }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------*/

void refresh_leds(void) //this is where the magic happens, writes the display
{
  unsigned char temp; //used to store pointers
  
  //Clear Latch
  Latch_Low;
  
  for (x = 0; x < 8; x++)
  {
    Clock_Low; //clear Clock in advance
    Data_Low;  //clear Data port in advance
    
    temp = *prt_dispmem; //Stores current display memory once this function is called, happens once per column
    
    //Set Data Outputs if necessary
    if (LUT_r[temp] > compare value) {Data_High};
    if (LUT_g[temp] > compare value) {Data_High};
    if (LUT_b[temp] > compare value) {Data_High};
    
    if ((x == row) {Ground_Active);
    
    
  

