#include "ConsoleInterface.h"

#include <utility>

namespace console {
void ConsoleInterface::Draw() {
  auto* wins = new Windows[3];
  int32_t ch;
  initscr();
  curs_set(0);
  start_color();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);
  init_pair(6, COLOR_RED, COLOR_BLACK);

  auto day_amount = static_cast<int8_t>(days_);
  int8_t start_day = 0;
  int32_t cur_city = 0;

  if (cities_.empty()) {
    PrintText(stdscr,
              100,
              "Waiting for correct input! Press any api_key if internet available or esc to rewrite config",
              red_on_black);
    while (cities_.empty()) {
      ch = getch();
      if (ch == esc_key && cities_.empty()) {
        endwin();
        active = false;
        delete[] wins;
        return;
      }
    }
  }

  clear();
  move(0, 0);

  DrawCity(wins, cities_[cur_city], start_day, day_amount);

  while ((ch = getch()) != esc_key) {
    switch (ch) {
      case right_arrow:start_day++;
        if (start_day > 15 - 1) {
          start_day = 15 - 1;
        }
        if (start_day + day_amount > 15 - 1) {
          start_day--;
        }
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
      case left_arrow:start_day--;
        if (start_day < 0) {
          start_day = 0;
        }
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
      case plus_key:day_amount++;
        if (day_amount > 3) {
          day_amount = 3;
        }
        if (start_day + day_amount > 15 - 1) {
          day_amount--;
          clear();
          move(0, 0);
        }
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
      case minus_key:day_amount--;
        if (day_amount < 0) {
          day_amount = 0;
        }
        clear();
        move(0, 0);
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
      case n_key:cur_city++;
        if (cur_city >= cities_.size()) {
          cur_city = 0;
        }
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
      case p_key:cur_city--;
        if (cur_city < 0) {
          cur_city = static_cast<int32_t>(cities_.size()) - 1;
        }
        DrawCity(wins, cities_[cur_city], start_day, day_amount);
        break;
    }
  }
  endwin();
  active = false;
  delete[] wins;
}

void ConsoleInterface::DrawDays(WINDOW** wins, const std::vector<forecast::Forecast*>& info, int32_t y_pos) {
  for (int32_t i = 0; i < info.size(); ++i) {
    if (wins[i] != nullptr) {
      delwin(wins[i]);
    }
    wins[i] = newwin(BLOCKHEIGHT, BLOCKWIDTH, BLOCK_Y(y_pos) + 2, BLOCK_X(i));
    ShowWindow(wins[i], kDaytime[i + max((1 - y_pos), 0) * 4], *info[i], Colors(i + 1));
  }
}

void ConsoleInterface::DrawHeadings(WINDOW** wins, const std::string& info, int32_t y_pos, const Colors& color) {
  auto win = wins[0];
  if (win != nullptr) {
    delwin(win);
  }
  int32_t width = (BLOCKWIDTH) + 10;
  win = newwin(3, width, 1 + y_pos * (BLOCKHEIGHT + 3), (BLOCKWIDTH * 4 - width) / 2);
  box(win, 0, 0);
  PrintText(win, width, info, color);
  wrefresh(win);
}

void ConsoleInterface::DrawCity(Windows* wins, forecast::city::CityInfo& city, int32_t from_day, int32_t amount) {
  DrawDays(wins[0].header, std::vector<forecast::Forecast*>({city.GetCurrentHour()}), 0);
  DrawHeadings(wins[0].city_heading, city.GetCityName(), 0, red_on_black);
  for (int32_t l = 0; l < amount; ++l) {
    DrawDays(wins[l].weather, city.GetDayInfo(from_day + l), l + 1);
    DrawHeadings(wins[l].day_naming, city.GetDayAndWeekday(from_day + l), l + 1, red_on_black);
  }
}

void ConsoleInterface::PrintWeather(WINDOW* win, int32_t from_x, int32_t from_y, const forecast::Forecast& weather) {
  int32_t x = from_x;
  int32_t y = from_y;
  auto name_of_weather = weather.GetNameFromWeatherCode();
  auto icon = weather.GetIconFromWeatherCode();
  if (icon == forecast::sunny || icon == forecast::pochti_sunny)
    wattron(win, COLOR_PAIR(yellow_on_black));
  else
    wattron(win, COLOR_PAIR(cyan_on_black));

  for (char c : forecast::kArts[icon]) {
    if (c == '\n') {
      y++;
      x = from_x;
    } else {
      mvwaddch(win, y, x, c);
      x++;
    }
  }
  if (icon == forecast::sunny || icon == forecast::pochti_sunny)
    wattroff(win, COLOR_PAIR(yellow_on_black));
  else
    wattroff(win, COLOR_PAIR(cyan_on_black));

  from_x += 13;
  mvwprintw(win, from_y + 0, from_x, name_of_weather.c_str());
  mvwprintw(win, from_y + 1, from_x, "%.1f (%.1f) °C", weather.GetTemperatureTrue(), weather.GetTemperatureFeel());
  mvwprintw(win, from_y + 2, from_x, "%.0f° %.1fkm/h", weather.GetWindDirection(), weather.GetWindSpeed());
  mvwprintw(win, from_y + 3, from_x, "%.1f m", weather.GetVisibility());
  mvwprintw(win, from_y + 4, from_x, "%.1f mm | %.1f %", weather.GetPrecipitation(), weather.GetEvapotranspiration());
}

void ConsoleInterface::ShowWindow(WINDOW* win,
                                  const std::string& text,
                                  const forecast::Forecast& weather,
                                  const Colors& color) {
  int32_t height, width;
  getmaxyx(win, height, width);
  box(win, 0, 0);
  mvwhline(win, 2, 1, ACS_HLINE, width - 2);
  PrintText(win, width, text, color);
  PrintWeather(win, 1, 3, weather);
  wrefresh(win);
}

void ConsoleInterface::PrintText(WINDOW* win, int32_t width, const std::string& string, const Colors& color) {
  int32_t x = static_cast<int32_t>(width - string.size()) / 2 + 2, y = 1;
  if (win == nullptr)
    win = stdscr;
  wattron(win, COLOR_PAIR(color));
  mvwprintw(win, y, x, "%s", string.c_str());
  wattroff(win, COLOR_PAIR(color));
  refresh();
}
void ConsoleInterface::SetCities(std::vector<forecast::city::CityInfo> cities) {
  cities_ = std::move(cities);
}

ConsoleInterface::ConsoleInterface(uint8_t days) : days_(days) {
  if (days_ > 3) {
    days_ = 3;
  }
}
} //console
