#include "fm_station.h"
#include "timetable.h"
#include <TFT.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
// SPI LCD pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8
/// DSP radio address
#define RADIO 0x10



// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
// create an instance of RTC
    tmElements_t tm;
// char array to print to the screen
char c_hou[3],c_minu[3],c_sec[3],c_freq[10],c_freq2[2],c_rssi[4];

volatile int mask,recmode,mode_set;
unsigned int i,hou,minu,sec,rssi,pgalevel_rf,pgalevel_if,old_rssi;
unsigned long rssi_count, rssi_count2;
int j,k,ref,p,q,r;
int old_hou,old_minu,count;
char buffer[20];
int read_byte,raw_upper,upper,lower,mode,old_mode;
float freq,old_freq;
int channel_num,s_upper,s_lower,hoge,initial_num,band_disp;
char s_upper2, s_lower2;
int terminal_1  = 2;
int terminal_2  = 4;
volatile char old_state = 0;
volatile int encorder_val  = 0;
int ct,pt,event,event2;
volatile int band_mode = LOW;
float listen_freq;







void setup() {
  unsigned int upper,lower,raw_upper;
  unsigned int mask,mode,mode_set,hoge,intfreq;
  Wire.begin() ;
  TFTscreen.begin();
  attachInterrupt(0, Rotary_encorder, CHANGE);
  attachInterrupt(1,mode_setting, CHANGE);
//    lcd.begin(16,2);
  pinMode(terminal_1, INPUT);
  pinMode(terminal_2, INPUT);
  pinMode(3, INPUT);
  digitalWrite(terminal_1, HIGH);
  digitalWrite(terminal_2, HIGH);


// clear the screen with 
//B   G   R 149,18,18 background blue
  TFTscreen.background(149, 18, 18);
  TFTscreen.setRotation(0); //Set the vertical orientation  
  // write the static text to the screen
  // set the font color to 
//B G R 17,234,237 character color
  TFTscreen.stroke(17,234,237);
//drawing borderline
  TFTscreen.line(0,25, TFTscreen.width(), 25);
  TFTscreen.line(0,120, TFTscreen.width(), 120);
  TFTscreen.rect(0,0,TFTscreen.width(),TFTscreen.height());
// set the font size
  TFTscreen.setTextSize(1);
  TFTscreen.text("~Radio Stations~",15,30);
  
// initial clock reading
    if (RTC.read(tm)) {
  hou=tm.Hour;
  minu=tm.Minute;
  sec=tm.Second;
  }else{
  hou=0;
  minu=0;
  }
// initialize several parameters  
  j=0;
  k=0;
  p=0;
  q=0;
  rssi_count=0;
  old_minu=61;
// initialize radio
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00        
    i2c_write(RADIO,0x01,0b00010011); ///AM-band
    i2c_write(RADIO,0x04,0x00);
    i2c_write(RADIO,0x05,0xff);
    listen_freq=1008.0; ////frequency MHz for FM, kHz for AM
    channel_num=listen_freq/3; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    
    mode_set=0;  
    event=0;
    event2=0;
    rssi=0;
    rssi_count=0;
    rssi_count2=0;
  i2c_read(RADIO,0x18);
  pgalevel_rf=(read_byte&0b11100000)>>5;
  pgalevel_if=(read_byte&0b00011100)>>2;
///setup end  
}

void loop() {
//  mode=0;
  int intfreq,ffreq;
  float tmp;
  display_clock(); /// displaying current time
  rssi_display(); /// displaying RSSI by the color of "@" in the display


if(event==1){  /// when intterupt by rotary encorder or band switch occurs
      switch(mode_set){
        case 0://AM
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    i2c_write(RADIO,0x01,0b00010011); ///AM-band   
    listen_freq=1008.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/3; 
    channel_num=initial_num+encorder_val*3; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    old_mode=mode;
    mode=0; 
        break;
        case 1://FM
    i2c_write(RADIO,0x01,0b00010011); ///FM-band Japan       
    i2c_write(RADIO,0x00,0b11000000); ///power_on,FM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=83.3; ////frequency MHz for FM, kHz for AM
    initial_num=(listen_freq-30)*40; 
    channel_num=initial_num+encorder_val*4; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b11100000);
    i2c_write(RADIO,0x00,0b11000000);
    i2c_write(RADIO,0x06,0b11111100);
    old_mode=mode;
    mode=1; 
        break;
        case 2://SW2
    i2c_write(RADIO,0x01,0b00110011); ///SW_2       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=3500.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val; 
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    old_mode=mode;   
          mode=0;  
        break;         
        case 3://SW3
    i2c_write(RADIO,0x01,0b00111011); ///SW_3       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=5000.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val; 
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    old_mode=mode;   
    mode=0;  
        break; 
               case 4://SW4
    i2c_write(RADIO,0x01,0b01000011); ///SW4       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=6000.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    old_mode=mode;
    mode=0; 
        break;  

    case 5://SW5
    i2c_write(RADIO,0x01,0b01000011); ///SW5       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=7000.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
//   old_mode=mode; 
         mode=0; 
    break; 

    case 6://SW6
    i2c_write(RADIO,0x01,0b01001011); ///SW6       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=9500.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
         mode=0;   
    break; 
                      case 7://SW7
    i2c_write(RADIO,0x01,0b01010011); ///SW7       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=11500.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
//   old_mode=mode; 
         mode=0;   
        break;
                      case 8://SW8
    i2c_write(RADIO,0x01,0b01011011); ///SW8       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=14000.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
//   old_mode=mode; 
         mode=0;   
        break;
                      case 9://SW9
    i2c_write(RADIO,0x01,0b01100011); ///SW9       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=15400.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
//   old_mode=mode; 
         mode=0;   
        break;  
                      case 10://LW
    i2c_write(RADIO,0x01,0b00000011); ///LW       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=234.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/3; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01100000; // AM, 3kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
//   old_mode=mode; 
        mode=0; 
   
        break;            
   
}

 broadcast_info(mode);

//// frequency infomation
  if(band_disp==0){
  switch(mode_set){
  case 0:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("LW: 150-285 kHz", 5, 125);

  TFTscreen.stroke(0,0,255);
  TFTscreen.setTextSize(1);
  TFTscreen.text("MW: 522-1620 kHz", 5, 125);  
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(2);
  TFTscreen.text("kHz", 80, 140); 
  break;
  case 1:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("MW: 522-1620 kHz", 5, 125);
  TFTscreen.setTextSize(2);
  TFTscreen.text("kHz", 80, 140); 
  TFTscreen.stroke(0,255,0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("FM: 76-108 MHz", 5, 125);
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(2);
  TFTscreen.text("MHz", 80, 140);
  break; 
   case 2:
  TFTscreen.stroke(149,18,18);

  TFTscreen.setTextSize(2);
  TFTscreen.text("MHz", 80, 140);
    TFTscreen.setTextSize(1);
  TFTscreen.text("FM: 76-108 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(2);
  TFTscreen.text("kHz", 80, 140); 
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW1: 3.2-4.1 MHz", 5, 125);
  break; 
   case 3:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW1: 3.2-4.1 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW2: 4.7-5.6 MHz", 5, 125);
  break; 
    case 4:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW2: 4.7-5.6 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW3: 5.7-6.4 MHz", 5, 125);
  break;  
  case 5:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW3: 5.7-6.4 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW4: 6.8-7.6 MHz", 5, 125);
  break; 
    case 6:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW4: 6.8-7.6 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW5: 9.2-10.0 MHz", 5, 125);
  break; 
    case 7:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW5: 9.2-10.0 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW6: 11.4-12.2 MHz", 5, 125);
  break; 
    case 8:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW6: 11.4-12.2 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW7: 13.5-14.3 MHz", 5, 125);
  break; 
    case 9:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW7: 13.5-14.3 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW8: 15-15.9 MHz", 5, 125);
  break; 
  case 10:
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(1);
  TFTscreen.text("SW8: 15-15.9 MHz", 5, 125); 
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(1);
  TFTscreen.text("LW: 150-285 kHz", 5, 125);
  break; 
  }
  band_disp=1;
  }

  old_freq=freq;
  hoge=channel_num;
if(mode==1){
  freq=int(hoge)*0.025+30.0;
}else{
////AM no sekai
if(mode_set==10){
//  lcd.print("LW");
    freq=int(hoge)*3; 
}
  else{ if(mode_set>=2){
//lcd.print("SW");
  freq=int(hoge)*5;  
  }else{
//lcd.print("AM");
  freq=int(hoge)*3;
}}}

  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(2);
  TFTscreen.text(".", 50, 140);  
  TFTscreen.text("0", 60, 140); 


  intfreq=old_freq;
  tmp=(old_freq-intfreq)*10;
  ffreq=tmp;
  TFTscreen.stroke(149,18,18);
  String n_freq2= String(intfreq);
  n_freq2.toCharArray(c_freq, 6);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_freq, 20, 140);
  
  if(mode_set==1)
  {
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(2);
  TFTscreen.text(".", 50, 140);  
  String n_freq3= String(ffreq);
    n_freq3.toCharArray(c_freq2, 2);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_freq2, 60, 140);  
  
  intfreq=freq;
  tmp=(freq-intfreq)*10;
  ffreq=tmp;
  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(2);
  TFTscreen.text(".", 50, 140);  
  String n_freq4= String(ffreq);
    n_freq4.toCharArray(c_freq2, 2);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_freq2, 60, 140); 
  }
  
  
  
  intfreq=freq;
//  tmp=(freq-intfreq)*10;
//  ffreq=tmp;
  TFTscreen.stroke(17,234,237);
  String n_freq= String(intfreq);
    n_freq.toCharArray(c_freq, 6);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_freq, 20, 140);

event=0;
}

}




void broadcast_info(int band)
{
  if(mode_set==1){
    if(k==0){
  for (int i = 0; i < sw_num; i++)
  {
    q=hou*sw_num+i;
    strcpy_P(buffer, (char*)pgm_read_word(&(sw_stations[q]))); 
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(149,18,18);
    TFTscreen.text(buffer, 3, (i+2)*10+30);
    j=0;

  }
  }
  k=1;
  }

  if(mode_set==2){
  if(k==0){
  for (int i = 0; i < fm_num; i++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(fm_stations[i]))); 
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(149,18,18);
    TFTscreen.text(buffer, 3, (i+2)*10+30);
    p=0;
  }
  }
  k=1;  
  }



  if(mode==1){                      ///FM (mode=1)
  if(j==0){
  for (int i = 0; i < fm_num; i++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(fm_stations[i]))); 
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(17,234,237);
    TFTscreen.text(buffer, 3, (i+2)*10+30);
  }
  j=1;
  }}else{                           ///AM (SW) (mode=0)
  if(p==0){
    hou=tm.Hour;

  for (int i = 0; i < sw_num; i++)
  {
    q=hou*sw_num+i;
    strcpy_P(buffer, (char*)pgm_read_word(&(sw_stations[q]))); 
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(17,234,237);
    TFTscreen.text(buffer, 3, (i+2)*10+30);
  }
  p=1;  
  }
}

}


void i2c_write(int device_address, int memory_address, int value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
//  delay(10);
  Wire.write(value);
//  delay(10);
  Wire.endTransmission();
//  delay(10);
}

void i2c_read(int device_address, int memory_address)
{
Wire.beginTransmission(device_address);
Wire.write(memory_address);
Wire.endTransmission(false);
Wire.requestFrom(device_address, 1);
read_byte = Wire.read();
Wire.endTransmission(true);
delay(2);
}



void display_clock(){
  
  if (RTC.read(tm)) {
  hou=tm.Hour;
  minu=tm.Minute;
  sec=tm.Second;
  }
  if(old_minu!=minu){
//  if(r==0){
    //erasing old time
  TFTscreen.stroke(149,18,18);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_hou, 30, 5);
  TFTscreen.text(":",55,5);
  TFTscreen.text(c_minu, 70, 5);
//  }  
  //writing new time
  TFTscreen.stroke(149,18,18);
  String n_hou= String(hou);
  String n_minu= String(minu);
  String n_sec= String(sec);
  k=1;  

  n_hou.toCharArray(c_hou, 5);
  n_minu.toCharArray(c_minu, 5);
  n_sec.toCharArray(c_sec, 5);  

  TFTscreen.stroke(17,234,237);
  TFTscreen.setTextSize(2);
  TFTscreen.text(c_hou, 30, 5);
  TFTscreen.text(":",55,5);
  TFTscreen.text(c_minu, 70, 5);
  old_minu=minu;
  }
}

void mode_setting(){
int sw;
ct=millis();
delay(10);
sw=digitalRead(3);
if(sw==LOW && (ct-pt)>50){
band_mode=HIGH;
mode_set=mode_set+1;
}
pt=ct;
if(mode_set>10){
  mode_set=0;
}
event=1;
encorder_val=0;
k=0;
band_disp=0;
}

void Rotary_encorder(void)
{
  if(!digitalRead(terminal_1)){
//    delay(20);
    if(digitalRead(terminal_2)){
      old_state = 'R';
    } else {
      old_state = 'L';
    }
  } else {
    if(digitalRead(terminal_2)){
      if(old_state == 'L'){ 
        encorder_val--;
      }
    } else {
      if(old_state == 'R'){
        encorder_val++;
      }
    }
    old_state = 0;
    event=1;
  }

}

void rssi_display(){
//// display RSSI by the color of "@", renewing every seconds. 
  rssi_count=millis()-1000*rssi_count2; 
  if(rssi_count>1000){
    rssi_count2++;
  i2c_read(RADIO,0x1B);
  old_rssi=rssi;
  rssi=read_byte&0b01111111;
  i2c_read(RADIO,0x18);
  pgalevel_rf=(read_byte&0b11100000)>>5;
  pgalevel_if=(read_byte&0b00011100)>>2;
  if(mode==1){
  rssi=103-rssi-6*pgalevel_rf-6*pgalevel_if;
  
  }else{
  rssi=103-rssi-6*pgalevel_rf-6*pgalevel_if;
  }
  TFTscreen.stroke(149,18,18); ///background color
  TFTscreen.setTextSize(2);
  String o_rssi= String(old_rssi);
  o_rssi.toCharArray(c_rssi, 4);
//  TFTscreen.text(c_rssi, 100, 130);  
  TFTscreen.text("@",110,125);
  if(rssi<20){
  TFTscreen.stroke(252,243,0);
  }else if(rssi<25){
      TFTscreen.stroke(0,252,20);
  }else if(rssi<30){
      TFTscreen.stroke(0,255,255);
  }else if(rssi<60){
          TFTscreen.stroke(0,0,255);
  }  
  TFTscreen.setTextSize(2);
  String n_rssi= String(rssi);
    n_rssi.toCharArray(c_rssi, 4);
  TFTscreen.text("@",110,125);
//  TFTscreen.text(c_rssi, 100, 130);  
//  rssi_count=0;
/////////////
    rssi_count=0;
}
}
