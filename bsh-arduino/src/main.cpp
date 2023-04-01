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

void reset_led(byte x, byte y, ColorType color_type) {
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

void set_led(byte x, byte y, size_t r, size_t g, size_t b) {
  ColorType color_type = get_color_type(r, g, b);
  RowSelection row = get_row(x);

  int mask = color_type << y * 3;
  byte low = mask & 0xFF;
  byte high = (mask >> 8) & 0xFF;

  command(row, CommandType::CHANNEL_HIGH_TOGGLE, high);
  command(row, CommandType::CHANNEL_LOW_TOGGLE, low);

  channels[x][y] = color_type;
}

void setup() {
  init_transfer();
  init_led();
  init_channels();
}

void loop() {
}