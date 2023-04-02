#include <Wire.h>
#include <LSM6DSOSensor.h>

void set_led_new(byte x, byte y, size_t r, size_t g, size_t b);

HardwareTimer timer(TIM1);

int mode = 8;

#define DEV_I2C Wire
#define SerialPort Serial
LSM6DSOSensor AccGyr(&DEV_I2C);

int val1 = 0, val2 = 0, val3 = 0;
int trial = 0;
int32_t gyroscope[3];

int32_t accelerometer[3];

void turn_on_all_leds() {
  // Serial.println("Turn ON!");
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j <4; j++) {
      set_led_new(i, j, 10, 10, 10);
    }
  }

}

void turn_off_all_leds() {
  // Serial.println("Turn OFF!");
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j <4; j++) {
      set_led_new(i, j, 0, 0, 0);
    }
  }
}

bool shaked = false;

void PoolSensorValues() {
    // Read accelerometer and gyroscope.
    AccGyr.Get_X_Axes(accelerometer);
    AccGyr.Get_G_Axes(gyroscope);


   // SerialPort.print(" | Acc[mg]: ");
   // SerialPort.print(accelerometer[0]);
   // SerialPort.print(" ");
   // SerialPort.println(accelerometer[1]);
   // SerialPort.print(" ");
   // SerialPort.print(accelerometer[2]);

    
    double var = sqrt(gyroscope[0] * gyroscope[0] + gyroscope[1] * gyroscope[1] + gyroscope[2] * gyroscope[2]);
    if(var > 30000){
      trial++;
    }
    if(trial > 100) {
      shaked = !shaked;
      if (shaked) {
        turn_on_all_leds();
      } else {
        turn_off_all_leds();
      }
      trial = 0;
      }
}
enum RowSelection {
  FIRST = 0x58,
  SECOND = 0x59,
  THIRD = 0x5A,
  FOURTH = 0x5B,
  ALL = 0x5C
};

const byte DEVICE_TOGGLE = 0x01;
const byte CHANNEL_LOW_TOGGLE = 0x02;
const byte CHANNEL_HIGH_TOGGLE = 0x03;
const byte CONFIGURATION_REGISTER = 0x04;

enum Values {
  FALSE,
  TRUE,
  START_PATTERN = 64 + 8 + 0,
  GYRO = 5, 
  GYROREMOTE = 6
};

struct Color {
  size_t red;
  size_t green;
  size_t blue;
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

enum DirectionalFlow {
  DOWN = 0,
  UP = 1,
  LEFT = 2,
  RIGHT = 3,
};



const byte ROWS = 4;
const byte COLUMNS = 4;
void command(RowSelection selection, byte command_type, byte value) {
  timer.pause();
  Wire.beginTransmission(selection);
  Wire.write(command_type);
  Wire.write(value);
  Wire.endTransmission();
  timer.resume();
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
  command(RowSelection::ALL, DEVICE_TOGGLE, TRUE);
  command(RowSelection::ALL, CHANNEL_LOW_TOGGLE, FALSE);
  command(RowSelection::ALL, CHANNEL_HIGH_TOGGLE, FALSE);
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

void reset_led(byte x, byte y) {
  RowSelection row = get_row(x);

  int mask = 0;
  for (byte i = 0; i < COLUMNS; i++)
  {
    mask <<= 3;
    mask += ColorType::RED_GREEN_BLUE;
  }
  
  byte low = mask & 0xFF;
  byte high = (mask >> 8) & 0xFF;

  command(row, CHANNEL_HIGH_TOGGLE, high);
  command(row, CHANNEL_LOW_TOGGLE, low);
}

const byte PATTERN_ORIGIN = 0x1E;
byte get_pattern_low_register(byte y) {
  return PATTERN_ORIGIN + y * 2;
}

byte to_low(size_t value) {
  return value & 0xFF;
}

byte to_high(size_t value) {
  return (value >> 8) & 0xFF;
}

void dim_led() {
  for (size_t i = 0; i < 24; i++)
  {
    command(RowSelection::ALL, 0x1E + i, 0x00);
    command(RowSelection::ALL, 0x1F + i, 0x00);
    command(RowSelection::ALL, CONFIGURATION_REGISTER, Values::START_PATTERN);
  }
}

DirectionalFlow direction_flow = DirectionalFlow::DOWN;

void set_led_inner(byte x, byte y, size_t r, size_t g, size_t b) {
  if (mode == 5 && (val1 > 20 || val2> 20 || val3 > 20)) {
    r /= 2;
        g /= 2;
            b /= 2;
  }

  RowSelection row = get_row(x);
  command(row, 0x1E + y * 6, to_low(r));
  command(row, 0x1F + y * 6, to_high(r));
  command(row, CONFIGURATION_REGISTER, Values::START_PATTERN);

  command(row, 0x1E + 2 + y * 6, to_low(g));
  command(row, 0x1F + 2 + y * 6, to_high(g));
  command(row, CONFIGURATION_REGISTER, Values::START_PATTERN);

  command(row, 0x1E + 4 + y * 6, to_low(b));
  command(row, 0x1F + 4 + y * 6, to_high(b));
  command(row, CONFIGURATION_REGISTER, Values::START_PATTERN);
}

void set_led_new(byte x, byte y, size_t r, size_t g, size_t b) {
  switch (direction_flow)
  {
    case DirectionalFlow::DOWN:
      set_led_inner(x, y, r, g, b);
      break;
    case DirectionalFlow::UP:
      set_led_inner(x, COLUMNS - y - 1, r, g, b);
      break;
    case DirectionalFlow::LEFT:
      set_led_inner(y, COLUMNS - x - 1, r, g, b);
      break;
    case DirectionalFlow::RIGHT:
      set_led_inner(y, x, r, g, b);
      break;
    default:
      Serial.println("bad flow");
      break;
  }
}



void setup() {
  DEV_I2C.begin();
  
  SerialPort.begin(9600);
  
  AccGyr.begin();
  AccGyr.Enable_X();
  AccGyr.Enable_G();
  
  // Configure timer
  timer.setPrescaleFactor(2564); // Set prescaler to 2564 => timer frequency = 168MHz/2564 = 65522 Hz (from prediv'd by 1 clocksource of 168 MHz)
  //timer.setOverflow(32761); // Set overflow to 32761 => timer frequency = 65522 Hz / 32761 = 2 Hz
  timer.setOverflow(100);
  timer.attachInterrupt(PoolSensorValues);
  timer.refresh(); // Make register changes take effect
  timer.resume(); // Start

  init_transfer();
  init_led();
  for (byte i = 0; i < COLUMNS; i++)
  {
    for (byte j = 0; j < ROWS; j++)
    {
      reset_led(i, j);
    }
  }
  dim_led();
}

size_t absol(int32_t value) {
  if (value < 0) {
    return -value;
  }
  return value;
}
const size_t TICK_LENGTH = 10; // millis?
const size_t LOOP_LENGTH = 50000;
const size_t LOOP_LENGTH_TICKS = LOOP_LENGTH / TICK_LENGTH;
size_t INCREMENT = 0;
const size_t COLOR_MAX = 1000;
const size_t TICKS_PER_COLOR = COLOR_MAX / TICK_LENGTH;
const float EPSILON = 5;

bool advance_color_transition(size_t* current_value, bool increase) {
  size_t inc = INCREMENT;
  bool overflow = (increase && (*current_value + inc) > COLOR_MAX)
    || (!increase && *current_value <= EPSILON + inc);
  if (overflow) {
    increase = !increase;
  }
  
  if (increase) {
    *current_value += inc;
  }
  else {
    *current_value -= inc;
  }

  return overflow;
}

// ---------------- Challenge 1 --------------------------
size_t color_value = 0;
bool increase = true;
int color_selector = 0;

void all_leds_smooth_transition() {
  Color color;
  switch (color_selector) {
    case 0:
      color = { color_value, 0, 0 };
      break;
    case 1:
      color = { 0, color_value, 0 };
      break;
    case 2:
      color = { 0, 0, color_value };
      break;
    default:
      color = {color_value, color_value, color_value};
      break;
  };

  for (size_t i = 0; i < ROWS; i++)
  {
    for (size_t j = 0; j < COLUMNS; j++)
    {
      set_led_new(i, j, color.red, color.green, color.blue);
    }
  }
  
  bool swap_direction = advance_color_transition(&color_value, increase);
  if (swap_direction) {
    increase = !increase;
    if (increase) {
      color_selector = (color_selector + 1) % 3;
    }
  }

  //Serial.printf("red %d, green %d, blue %d\n", color.red, color.green, color.blue);
} 


// ------------------------------------------------------


// ------------------------ Challenge 2 --------------------
const size_t OFFSET = 250;
size_t row_value[4] = {0, OFFSET, 2*OFFSET, 3*OFFSET};
bool row_increase[4] = {true, true, true, true};
int row_color_selector[4] = {0, 0, 0, 0};

Color get_color_for_row(int row) {
  Color color;

  size_t value = row_value[row];
  size_t value_4 = value / 4;
  //Serial.printf("val %d val4 %d\n", value, value_4);

  int color_selector = row_color_selector[row];
  switch (color_selector) {
    case 0:
      color = { value, 0, 0 };
      break;
    case 1:
      color = { 0, value, 0 };
      break;
    case 2:
      color = { 0, 0, value };
      break;
    default:
      color = {value, value, value};
      break;
  };

  return color;
}



Color letter_color = {500, 300, 300};
const bool BSH_LETTERS[15][4] = {
  {false, false, false, false},

  // h
  {true, false, false, true},
  {true, true, true, true},
  {true, false, false, true},
  {true, false, false, true},

  {false, false, false, false},

  // s
  {false, true, true, true},
  {true, false, false, false},
  {false, false, false, true},
  {true, true, true, false},

  {false, false, false, false},

  // b
  {true, true, true, true}, 
  {true, false, false, true},
  {true, true, true, true},
  {false, false, false, true},
};

size_t letter_move_counter = 0;
int letter_row_index = 0;

void falling_letters() {
  for (int x = 0; x < ROWS; x++) {
    int letter_index = 14 - letter_row_index - x;
    if (letter_index < 0) {
      letter_index += 15;
    }
    auto letter_row = BSH_LETTERS[letter_index];
    
    for (int y = 0; y < COLUMNS; y++) {
      if (letter_row[y]) {
        set_led_new(x, y, letter_color.red, letter_color.green, letter_color.blue);
      }
      else {
        //set_led_new(x, y, 0, 0, 0);
      }
    }
  }

  if (letter_move_counter == 20) {
    letter_row_index = (letter_row_index + 1) % 15;
    letter_move_counter = 0;
  }

  letter_move_counter++;
}

void color_row(byte x, size_t r, size_t g, size_t b, Values value) {
    int letter_index = 14 - letter_row_index - x;
    if (letter_index < 0) {
      letter_index += 15;
    }

    auto letter_row = BSH_LETTERS[letter_index];
    for (size_t i = 0; i < COLUMNS; i++)
    {
      if (letter_row[i] == false || value == Values::FALSE) {
        set_led_new(x, i , r, g, b);
      }
    };

}

void falling_rows(Values value) {

  if (value == Values::GYRO) {

  INCREMENT = absol(accelerometer[1])/10;
  if (accelerometer[1] > 500) { 
    direction_flow = DirectionalFlow::RIGHT;
  } else if (accelerometer[1] < -500) {
    direction_flow = DirectionalFlow::LEFT;
  } else {
    direction_flow = DirectionalFlow::DOWN;
  } 
  } else if (value == Values::GYROREMOTE) {
  INCREMENT = min(absol(val2 + 100), (size_t)1000) / 10;
  if (val2 > 500) { 
    direction_flow = DirectionalFlow::RIGHT;
  } else if (val2 < -500) {
    direction_flow = DirectionalFlow::LEFT;
  } else {
    direction_flow = DirectionalFlow::DOWN;
  } 
  } else {
  INCREMENT = 60;
  }
  
  for (int i = 0; i < 4; i++) {
    auto row_color = get_color_for_row(i);
    //Serial.printf("red %d, green %d, blue %d\n", row_color.red, row_color.green, row_color.blue);
    color_row(i, row_color.red, row_color.green, row_color.blue, value);
    //Serial.println(i);

    bool swap_direction = advance_color_transition(&row_value[i], row_increase[i]);
    if (swap_direction) {
      row_increase[i] = !row_increase[i];
      if (row_increase[i]) {
        row_color_selector[i] = (row_color_selector[i] + 1) % 3;
      }
    }
  }
} 
// ----------------------------------------------------------------------

void shake_mode(int mode) {
    if (mode == 1) {
      shaked = !shaked;
      if (shaked) {
        turn_on_all_leds();
      } else {
        turn_off_all_leds();
      }
    } 
    
}

void smooth_mode_board(int mode) {
    if (mode == 2 || mode == 5) {
      falling_rows(Values::FALSE);
    } 
    
}

void falling_gyro(int mode) {
    if (mode == 7 || mode == 3) {
      falling_rows(Values::GYRO);
      falling_letters();
    } 

}

void falling_gyro_remote(int mode) {
    if (mode == 4) {
      falling_rows(Values::GYROREMOTE);
      falling_letters();
    } 

}

void letters_mode_board(int mode) {
    if (mode == 8) {
      falling_rows(Values::TRUE);
      falling_letters();
    } 

}

void turn_off() {
    if (mode == 0) {
      dim_led();
      command(RowSelection::ALL, CHANNEL_HIGH_TOGGLE, FALSE);
      command(RowSelection::ALL, CHANNEL_LOW_TOGGLE, FALSE);
      command(RowSelection::ALL, DEVICE_TOGGLE, FALSE);
      mode = 42;
    } 

}

void turn_on() {
    if (mode == 10) {
      init_led();
      for (byte i = 0; i < COLUMNS; i++)
      {
        for (byte j = 0; j < ROWS; j++)
        {
          reset_led(i, j);
        }
      }
      dim_led();
      mode = 8;
      Serial.print("aa");
    } 

}


char buffer[64];
int first;

void loop() {
  letters_mode_board(mode);
  smooth_mode_board(mode);
  falling_gyro(mode);
  falling_gyro_remote(mode);
  turn_off();
  turn_on();
  // 


  first = Serial.read();

  if (first != -1 ) {
    buffer[0] = first;
    int count = Serial.readBytesUntil('\n', buffer+1, sizeof(buffer)-1);
    buffer[count] = '\0';
    
    // parse the buffer to extract the integers:
    sscanf(buffer, "%d_%d_%d_%d", &mode, &val1, &val2, &val3);

    shake_mode(mode);
    // print the received integers to the serial monitor:
  }
  //delay(100);
}