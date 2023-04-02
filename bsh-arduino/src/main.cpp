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

int row_to_index(RowSelection row) {
  switch (row) {
    case RowSelection::FIRST:
      return 0;
    case RowSelection::SECOND:
      return 1;
    case RowSelection::THIRD:
      return 2;
    case RowSelection::FOURTH:
      return 3;
    case RowSelection::ALL:
      return -1;
  };
}

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

const size_t TICK_LENGTH = 10; // millis?
const size_t LOOP_LENGTH = 50000;
const size_t LOOP_LENGTH_TICKS = LOOP_LENGTH / TICK_LENGTH;

const size_t COLOR_MAX = 8192; // 2^13
const size_t TICKS_PER_COLOR = COLOR_MAX / TICK_LENGTH;
const size_t INCREMENT = 1000;//LOOP_LENGTH_TICKS / (TICKS_PER_COLOR * TICKS_PER_COLOR * TICKS_PER_COLOR);

// ---------------- Challenge 1 --------------------------
bool increment_color_channel(size_t* value) {
  size_t original = *value;
  *value = (original + INCREMENT) % COLOR_MAX;

  bool overflow = (*value) < original;
  return overflow;
}

Color color = { 0, 0, 0 };

void all_leds_smooth_transition() {
  color_led(0, 0, color.red, color.green, color.blue); // TODO: color all leds

  bool update_green = increment_color_channel(&color.blue);
  if (update_green) {
    bool update_red = increment_color_channel(&color.green);
    if (update_red) {
      increment_color_channel(&color.red);
    }
  }
} 
// ------------------------------------------------------

Color row_colors[4] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

void color_row(RowSelection row, size_t r, size_t g, size_t b) {
  ColorType color_type = get_color_type(r, g, b);

  byte low = 0x00;
  low = color_type | low;
  low = (color_type << 3) | low;
  byte payload = 0xFF;
  
  command(row, CommandType::CHANNEL_HIGH_TOGGLE, payload);
  command(row, CommandType::CHANNEL_LOW_TOGGLE, payload);

}

// ------------------------ Challenge 2 --------------------
void falling_rows() {
  auto color = row_colors[0];
  color_row(RowSelection::FIRST, color.red, color.green, color.blue);
  color = row_colors[1];
  color_row(RowSelection::SECOND, color.red, color.green, color.blue);
  color = row_colors[2];
  color_row(RowSelection::THIRD, color.red, color.green, color.blue);
  color = row_colors[3];
  color_row(RowSelection::FOURTH, color.red, color.green, color.blue);

  row_colors[3] = row_colors[2];
  row_colors[2] = row_colors[1];
  row_colors[1] = row_colors[0];
  bool update_green = increment_color_channel(&row_colors[0].blue);
  if (update_green) {
    bool update_red = increment_color_channel(&row_colors[0].green);
    if (update_red) {
      increment_color_channel(&row_colors[0].red);
    }
  }
} 

Color letter_color = {5000, 3000, 3000};
const bool BSH_LETTERS[14][4] = {
  // b
  {true, false, false, false},
  {true, true, true, true},
  {true, false, false, true},
  {true, true, true, true},
  
  {false, false, false, false},

  // s
  {false, true, true, true},
  {true, false, false, false},
  {false, false, false, true},
  {true, true, true, false},

  {false, false, false, false},

  // h
  {true, false, false, true},
  {true, true, true, true},
  {true, false, false, true},
  {true, false, false, true},
};

bool letter_cells[ROWS][COLUMNS] = {
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false},
};

void falling_letters() {
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      // TODO
    }
  }
}
// ----------------------------------------------------------------------

void loop() {
  falling_rows();

  //delay(100);
}