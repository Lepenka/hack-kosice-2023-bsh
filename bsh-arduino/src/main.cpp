#include <Wire.h>

enum RowSelection {
  FIRST = 0x58,
  SECOND = 0x59,
  THIRD = 0x5A,
  FOURTH = 0x5B,
  ALL = 0x5C
};

enum CommandType {
  DEVICE_TOGGLE = 0x01,
  CHANNEL_LOW_TOGGLE = 0x02,
  CHANNEL_HIGH_TOGGLE = 0x03,
  CONFIGURATION_REGISTER = 0x04
};

enum Values {
  FALSE,
  TRUE
};

enum ColorType {
  NONE = 0,
  RED = 1,
  GREEN = 2,
  BLUE = 4,
  RED_GREEN = 3,
  RED_BLUE = 5,
  GREEN_BLUE = 6,
  RED_GREEN_BLUE = 7,
};

const byte ROWS = 4;
const byte COLUMNS = 4;

ColorType channels[ROWS][COLUMNS];
struct Color {
  size_t red;
  size_t green;
  size_t blue;
};

void command(RowSelection selection, CommandType command_type, byte value) {
  Wire.beginTransmission(selection);
  Wire.write(command_type);
  Wire.write(value);
  Wire.endTransmission();
}

void init_channels() {
  for (byte i = 0; i < ROWS; i++)
  {
    for (byte j = 0; j < COLUMNS; j++)
    {
      channels[i][j] = ColorType::NONE;
    }
  }
  Serial.println("Channels initialized");
}

void init_transfer() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial)
    delay(100);
  delay(1000);
  Serial.println("Arduino initialized");
}

// DEVICE_TOGGLE has to be enabled for CHANNEL_XX_TOGGLE to work 
void init_led() {
  command(RowSelection::ALL, CommandType::DEVICE_TOGGLE, TRUE);
  command(RowSelection::ALL, CommandType::CHANNEL_LOW_TOGGLE, FALSE);
  command(RowSelection::ALL, CommandType::CHANNEL_HIGH_TOGGLE, FALSE);
  Serial.println("Leds initialized");
}

RowSelection get_row(byte x) {
  switch (x) {
      case 0:
        return RowSelection::FIRST;
      case 1:
        return RowSelection::SECOND;
      case 2:
        return RowSelection::THIRD;
      case 3:
        return RowSelection::FOURTH;
      default:
        return RowSelection::ALL;
  }
}

ColorType get_color_type(size_t r, size_t g, size_t b) {
  byte value = 0;
  if (r > 0) value |= RED;
  if (g > 0) value |= GREEN;
  if (b > 0) value |= BLUE;
  return (ColorType)value;
}

ColorType diff_color_types(ColorType source, ColorType removal) {
  byte mask = ~((byte)removal);
  byte to_remove = (byte)source & mask;
  byte result = (byte)source - to_remove;
  return (ColorType)result;
}

void clear_led(byte x, byte y, ColorType color_type) {
  RowSelection row = get_row(x);

  ColorType old_color = channels[x][y];
  ColorType new_color = diff_color_types(old_color, color_type);

  channels[x][y] = new_color;

  int mask = 0;
  for (byte i = 0; i < COLUMNS; i++)
  {
    mask += channels[x][i];
    mask <<= 3;
  }
  
  byte low = mask & 0xFF;
  byte high = (mask >> 8) & 0xFF;

  command(row, CommandType::CHANNEL_HIGH_TOGGLE, high);
  command(row, CommandType::CHANNEL_LOW_TOGGLE, low);
}

void toggle_led(byte x, byte y, size_t r, size_t g, size_t b) {
  ColorType color_type = get_color_type(r, g, b);
  RowSelection row = get_row(x);

  int mask = color_type << y * 3;
  byte low = mask & 0xFF;
  byte high = (mask >> 8) & 0xFF;

  command(row, CommandType::CHANNEL_HIGH_TOGGLE, high);
  command(row, CommandType::CHANNEL_LOW_TOGGLE, low);

  channels[x][y] = color_type;
}

// byte get_pattern(byte x, byte y) {

// }

void color_led(byte x, byte y, size_t r, size_t g, size_t b) {
  toggle_led(x, y, r, g, b);
  //dim_led(x, y, r, g, b);
}

void setup() {
  init_transfer();
  init_led();
  init_channels();
}

const size_t INCREMENT = 100;
const size_t COLOR_MAX = 8192; // 2^13

size_t color[3] = { 0, 0, 0 };
int increment_index = 2;

void loop() {
  color_led(0, 0, color[0], color[1], color[2]);

  size_t original = color[increment_index];
  size_t updated = (original + INCREMENT) % COLOR_MAX;
  color[increment_index] = updated;

  if (updated < original) {
    if (increment_index == 0) {
      increment_index = 2;
    } else {
      increment_index--;
    }
  }

  delay(100);
}