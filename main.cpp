#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// On an arduino MEGA 2560: 20(SDA), 21(SCL)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET_PIN -1   // -1 if using Arduino reset pin
#define SCREEN_ADDRESS 0x3C // 128x32: 0x3C || 128x64: 0x3D

Adafruit_SSD1306 OLED_LCD(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

boolean gameover = false;

const int SNAKE_ROWS = 30; // snake segments (rows)
const int SNAKE_COLS = 2;  // x and y for each segment
const int snake_pos_x_pos = 0;
const int snake_pos_y_pos = 1;

int snake_pos[SNAKE_ROWS][SNAKE_COLS];
int food_pos[] = {0, 0}; // y, x (or row, col) of food


int h_dir = 1; // will be either 1 or -1 to control left/right
int v_dir = 0; // will be either 1 or -1 to control up/down

void checkSerial()
{
  if (Serial.available())
  {
    switch(Serial.read())
    {
    case 'w':
      // up
      v_dir = -1;
      h_dir = 0;
      Serial.println("w");
      break;
    case 'a':
      // left
      h_dir = -1;
      v_dir = 0;
      Serial.println("a");
      break;
    case 'd':
      // right
      h_dir = 1;
      v_dir = 0;
      Serial.println("d");
      break;
    case 's':
      // down
      v_dir = 1;
      h_dir = 0;
      Serial.println("s");
      break;
    default:
      break;
    }
  }
}


void moveSnake()
{
  OLED_LCD.clearDisplay();

  if (!gameover)
  {
    // let all non-head segments inherit the x,y values of the next
    // segment to give the illusion of movement
    for (int i = 0; i < SNAKE_ROWS-1; ++i)
    {
      snake_pos[i][snake_pos_x_pos] = snake_pos[i+1][snake_pos_x_pos];
      snake_pos[i][snake_pos_y_pos] = snake_pos[i+1][snake_pos_y_pos];
    }

    // now move the head
    snake_pos[SNAKE_ROWS-1][snake_pos_x_pos] += h_dir;
    snake_pos[SNAKE_ROWS-1][snake_pos_y_pos] += v_dir;

    // check if snake head is out of bounds
    if ( (snake_pos[SNAKE_ROWS-1][snake_pos_x_pos] < 0) || (snake_pos[SNAKE_ROWS-1][snake_pos_x_pos] > SCREEN_WIDTH) || 
         (snake_pos[SNAKE_ROWS-1][snake_pos_y_pos] < 0) || (snake_pos[SNAKE_ROWS-1][snake_pos_y_pos] > SCREEN_HEIGHT) )
    {
      gameover = true;
    }

    // check if snake head has hit the body
    int head_x = snake_pos[SNAKE_ROWS-1][snake_pos_x_pos];
    //Serial.print("head_x: "); Serial.println(head_x); // debug
    int head_y = snake_pos[SNAKE_ROWS-1][snake_pos_y_pos];
    //Serial.print("head_y: "); Serial.println(head_y); // debug

    for (int i = 0; i < SNAKE_ROWS-1; ++i)
    {
      if ( snake_pos[i][snake_pos_x_pos] == head_x && 
           snake_pos[i][snake_pos_y_pos] == head_y )
      {
        //Serial.print("fail_x: "); Serial.println(snake_pos[i][snake_pos_x_pos]); // debug
        //Serial.print("fail_y: "); Serial.println(snake_pos[i][snake_pos_y_pos]); // debug
        gameover = true;
        break;
      }
    }

  }

  if (!gameover)
  {
    // draw each segment of the snake
    for(int i = 0; i < SNAKE_ROWS; ++i)
    {
      OLED_LCD.drawPixel(snake_pos[i][snake_pos_x_pos], snake_pos[i][snake_pos_y_pos], SSD1306_WHITE);
    }

    OLED_LCD.display();
  } 
  else
  {
    OLED_LCD.setTextSize(2);
    OLED_LCD.setTextColor(SSD1306_WHITE);
    OLED_LCD.setCursor(10, 10);
    OLED_LCD.println("GAME OVER");
    OLED_LCD.display();
    for(;;);
  } 
}


void initSnake()
{
  for (int i = 0; i < SNAKE_ROWS; ++i)
  {
    snake_pos[i][snake_pos_x_pos] = 0;
    snake_pos[i][snake_pos_y_pos] = 0;
  }

  OLED_LCD.clearDisplay();
  OLED_LCD.setTextSize(2);
  OLED_LCD.setTextColor(SSD1306_WHITE);
  OLED_LCD.setCursor(1, 10);
  OLED_LCD.println("GET READY!");
  OLED_LCD.display();
  delay(2000);

  for (int i = 0; i < SNAKE_ROWS; ++i)
  {
    OLED_LCD.drawPixel( snake_pos[i][snake_pos_x_pos], snake_pos[i][snake_pos_y_pos], SSD1306_WHITE);
  }
}


void setup() 
{
  Serial.begin(9600);

  if(!OLED_LCD.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 Allocation Failed"));
    for(;;); // could not start LCD error condition, so loop forever
  }

  initSnake();
  OLED_LCD.display();
}


void loop() 
{
  checkSerial();
  moveSnake();
  delay(150);
}