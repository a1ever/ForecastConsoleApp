#ifndef WEATHER_LIB_CONSOLE_H_
#define WEATHER_LIB_CONSOLE_H_

#include "Forecast/Forecast.h"
#include "Forecast/City.h"
#include "HelpStructures.h"

namespace console {
class ConsoleInterface {
  std::vector<forecast::city::CityInfo> cities_;
  uint8_t days_ = 0;
 public:
  inline static bool active = true;
  explicit ConsoleInterface(uint8_t days);
  void Draw();
  static void DrawCity(Windows* wins, forecast::city::CityInfo& city, int32_t from_day, int32_t amount = 3);
  static void ShowWindow(WINDOW* win,
                         const std::string& text,
                         const forecast::Forecast& weather,
                         const Colors& color);
  static void PrintText(WINDOW* win, int32_t width, const std::string& string, const Colors& color);
  static void DrawDays(WINDOW** wins, const std::vector<forecast::Forecast*>& info, int32_t y_pos);
  static void DrawHeadings(WINDOW** wins, const string& info, int32_t y_pos, const Colors& color);
  static void PrintWeather(WINDOW* win, int32_t from_x, int32_t from_y, const forecast::Forecast& weather);
  void SetCities(std::vector<forecast::city::CityInfo> cities);
};
} //console

#endif