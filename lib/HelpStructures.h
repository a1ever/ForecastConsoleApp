#ifndef WEATHER_LIB_HELP_H_
#define WEATHER_LIB_HELP_H_

#include <string>
#include <vector>

#include <curses.h>

const int BLOCKWIDTH=30;
const int BLOCKHEIGHT=9;
const int BLOCK_HEIGHT_IN_TABLE=12;
const int BLOCK_START_X=1;
const int BLOCK_START_Y=1;

int BLOCK_Y(int x){
    return BLOCK_START_Y + x * BLOCK_HEIGHT_IN_TABLE;
}

int BLOCK_X(int x) {
    return (BLOCK_START_X + x * BLOCKWIDTH);
}

namespace console {
struct Windows {
  WINDOW* weather[4]{
      nullptr,
      nullptr,
      nullptr,
      nullptr
  };

  WINDOW* header[1]{
      nullptr
  };

  WINDOW* city_heading[1]{
      nullptr
  };
  WINDOW* day_naming[1]{
      nullptr
  };
};

enum Colors {
  white_on_black,
  magenta_on_black,
  green_on_black,
  blue_on_black,
  cyan_on_black,
  yellow_on_black,
  red_on_black
};

enum Keys {
  esc_key = 27,
  left_arrow = 260,
  right_arrow = 261,
  n_key = 110,
  p_key = 112,
  plus_key = 43,
  minus_key = 45
};

const std::vector<std::string> kDaytime{
    "Утро",
    "День",
    "Вечер",
    "Ночь",
    "Сейчас"
};
}

#endif