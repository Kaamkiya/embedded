#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Defines for joystick
#define X_PIN 34
#define Y_PIN 35
#define SW_PIN 26

// Defines for SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

// Map
const int world_map[10][10] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float posx = 5, posy = 5;
float dirx = -1, diry = 0;
float cam_planex = 0, cam_planey = 0.3;

void setup() {
  Serial.begin(9600);
  Wire.begin(21,22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (1); // Terminate
  }

  pinMode(SW_PIN, INPUT_PULLUP);
}

void render() {
    display.clearDisplay();

  for (int i = 0; i < SCREEN_WIDTH; i+=2) {
    float camx = 2 * i/float(SCREEN_WIDTH) - 1;
    float ray_dirx = dirx + cam_planex * camx;
    float ray_diry = diry + cam_planey * camx;

    int mapx = int(posx);
    int mapy = int(posy);

    float side_distx, side_disty;
    float delta_distx = (ray_dirx == 0) ? 9999 : abs(1/ray_dirx);
    float delta_disty = (ray_diry == 0) ? 9999 : abs(1/ray_diry);
    int stepx, stepy;
    bool hit = false;
    float perpendicular_wall_dist;
    char side;

    if (ray_dirx < 0) {
      stepx = -1;
      side_distx = (posx - mapx) * delta_distx;
    } else {
      stepx = 1;
      side_distx = (posx + 1 - mapx) * delta_distx;
    }
    if (ray_diry < 0) {
      stepy = -1;
      side_disty = (posy - mapy) * delta_disty;
    } else {
      stepy = 1;
      side_disty = (posy + 1 - mapy) * delta_disty;
    }

    while (!hit) {
      if (side_distx < side_disty) {
        side_distx += delta_distx;
        mapx += stepx;
        side = 'x';
      } else {
        side_disty += delta_disty;
        mapy += stepy;
        side = 'y';
      }

      if (world_map[mapy][mapx]) hit = true;
    }

    if (side == 'x') perpendicular_wall_dist = side_distx - delta_distx;
    else perpendicular_wall_dist = side_disty - delta_disty;

    // Serial.println(perpendicular_wall_dist);

    int height = int(SCREEN_HEIGHT / perpendicular_wall_dist);
    int topy = -height/2 + SCREEN_HEIGHT/2;
    int bottomy = height/2 + SCREEN_HEIGHT/2;
    if (topy < 0) topy = 0;
    if (bottomy > SCREEN_HEIGHT) bottomy = SCREEN_HEIGHT;

    display.drawLine(i, topy, i, bottomy, SSD1306_WHITE);
    display.drawLine(i+1, topy, i+1, bottomy, SSD1306_WHITE);
  }
  display.display();
}

void update() {
  float rotvel = 0.1; // rad/s
  float movespeed = 0.1;

  int stick_x = analogRead(X_PIN);
  int stick_y = analogRead(Y_PIN);

  if (stick_x > 2100) {
    float old_dirx = dirx;
    dirx = dirx * cos(-rotvel) - diry * sin(-rotvel);
    diry = old_dirx * sin(-rotvel) + diry * cos(-rotvel);
    float old_planex = cam_planex;
    cam_planex = cam_planex * cos(-rotvel) - cam_planey * sin(-rotvel);
    cam_planey = old_planex * sin(-rotvel) + cam_planey * cos(-rotvel);
  } else if (stick_x < 1500) {
    float old_dirx = dirx;
    dirx = dirx * cos(rotvel) - diry * sin(rotvel);
    diry = old_dirx * sin(rotvel) + diry * cos(rotvel);
    float old_planex = cam_planex;
    cam_planex = cam_planex * cos(rotvel) - cam_planey * sin(rotvel);
    cam_planey = old_planex * sin(rotvel) + cam_planey * cos(rotvel);
  }

  if (stick_y < 1800) {
    // forwards
    posx += dirx * movespeed;
    if (world_map[int(posy)][int(posx)] == 1) posx -= dirx * movespeed;
    posy += diry * movespeed;
    if (world_map[int(posy)][int(posx)] == 1) posy -= diry * movespeed;
  } else if (stick_y > 2000) {
    // backwards
  }

  Serial.print(posx);
  Serial.print(", ");
  Serial.println(posy);
}

void loop() {
  render();
  update();

  delay(1000 / 60.0);
}
