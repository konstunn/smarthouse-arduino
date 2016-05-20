
#include <SoftwareSerial.h>
#include "dht11.h"
#include "PCD8544.h"

#define  TEMPERATURE           0x01
#define  HUMIDITY              0x02
#define  ILLUMINATION          0x03
#define  TOGGLE_LCD_BACKLIGHT  0x0A

#define  DHT_ERROR_CHECKSUMM   0x04
#define  DHT_ERROR_TIMEOUT     0x05 
#define  UNKNOWN_SYMBOL        0x08
#define  UNKNOWN_ERROR         0x09

#define  LIGHT_SENSOR_PIN      A1
#define  DHT_PIN               8
#define  LED_PIN               13
#define  LCD_BACKLIGHT_PIN     9

#define DEGREE_CHARACTER       0

static PCD8544 lcd;
dht11 dht;

void writeWordToSerial(int src, Stream & serial) 
{
    byte h = highByte(src);
    byte l = lowByte(src);
    serial.write(l);
    serial.write(h);
}

int temp;
int humi;
int light;

void writeHumTempToSerial(byte status, byte request, int value, Stream & serial)
{
    switch (status) 
    {
        case DHTLIB_OK :
                temp = dht.temperature;
                humi = dht.humidity;
                serial.write(request);
                writeWordToSerial(value, serial);
                break;
        case DHTLIB_ERROR_CHECKSUM :
                serial.write(DHT_ERROR_CHECKSUMM);
                serial.write(-1); 
                break;
        case DHTLIB_ERROR_TIMEOUT :
                serial.write(DHT_ERROR_TIMEOUT);
                serial.write(-1);
        default :
                serial.write(UNKNOWN_ERROR);
                serial.write(status);
    }
}

SoftwareSerial softSerial(10,11);

static const byte degreeGlyph[] = { B00001100, 
                                    B00010010, 
                                    B00001100, 
                                    B00000000, 
                                    B00000000 };

void setup() 
{
    Serial.begin(9600);
    softSerial.begin(9600);
    
    pinMode(DHT_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    
    pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(LCD_BACKLIGHT_PIN, LOW);
    
    lcd.begin(84,48);
    lcd.createChar(0, degreeGlyph);
}

void serialEvent() 
{
    digitalWrite(LED_PIN, HIGH);
    while (Serial.available()) 
    {
        byte request = Serial.read();
        switch (request) 
        {
            case TEMPERATURE :
                    writeHumTempToSerial(dht.read(DHT_PIN), TEMPERATURE, dht.temperature, Serial);
                    break;
            case HUMIDITY :
                    writeHumTempToSerial(dht.read(DHT_PIN), HUMIDITY, dht.humidity, Serial);
                    break;
            case ILLUMINATION :
                    light = 1023 - analogRead(LIGHT_SENSOR_PIN);
                    Serial.write(ILLUMINATION);
                    writeWordToSerial(light, Serial);
                    break;
            default :
                    Serial.write(UNKNOWN_SYMBOL);
                    Serial.write(0);
                    break;
        }
    }
    digitalWrite(LED_PIN, LOW);
}

void toggleLcdBacklight() 
{
    if (digitalRead(LCD_BACKLIGHT_PIN) == HIGH)
        digitalWrite(LCD_BACKLIGHT_PIN, LOW);
    else
        digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
}

unsigned long ms = 0;

#define LCD_BACKLIGHT_BLINK

void loop() 
{     
    light = 1023 - analogRead(LIGHT_SENSOR_PIN);

    if (dht.read(DHT_PIN) == DHTLIB_OK)
    { 
        temp = dht.temperature;
        humi = dht.humidity;      
    }
    
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(temp);
    lcd.write(DEGREE_CHARACTER);
    lcd.print("C");
    
    lcd.setCursor(0,1);
    lcd.print("Humidity:");
    lcd.print(humi);
    lcd.print("%");
    
    lcd.setCursor(0,2);
    lcd.print("Illum:");
    lcd.print(light);
    lcd.print("?");
 
    delay(1000);
    
    #ifdef LCD_BACKLIGHT_BLINK
    if (abs(millis() - ms) > 10000)        
    {  
        lcd.clear();
        lcd.setCursor(0,2);
        lcd.print("    HELLO!");
        toggleLcdBacklight();
        lcd.setInverse(true);
        delay(500);
        toggleLcdBacklight();
        lcd.setInverse(false);
        delay(500);
        toggleLcdBacklight();
        lcd.setInverse(true);
        delay(500);
        toggleLcdBacklight();
        lcd.setInverse(false);
        ms = millis();
        lcd.clear();
    }
    #endif

    noInterrupts();
    while (softSerial.available()) 
    {
        digitalWrite(LED_PIN, HIGH);
        switch (softSerial.read()) 
        {
            case TEMPERATURE :
                    writeHumTempToSerial(DHTLIB_OK, TEMPERATURE, temp, softSerial);
                    break;
            case HUMIDITY :
                    writeHumTempToSerial(DHTLIB_OK, HUMIDITY, humi, softSerial);
                    break;
            case ILLUMINATION :
                    light = 1023 - analogRead(LIGHT_SENSOR_PIN);
                    softSerial.write(ILLUMINATION);
                    writeWordToSerial(light, softSerial);
                    break;
            case TOGGLE_LCD_BACKLIGHT :
                    toggleLcdBacklight();
                    break;
            default :
                    softSerial.write(UNKNOWN_SYMBOL);
                    softSerial.write(1);
                    break;
        }
    }
    digitalWrite(LED_PIN, LOW);
    interrupts();
}
