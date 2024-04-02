#ifndef WEATHER_LIB_HOURLY_H_
#define WEATHER_LIB_HOURLY_H_

#include <boost/date_time.hpp>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <ctime>
#include <utility>
#include <ctime>
#include <cstdint>
#include <map>
#include <vector>

using std::string;

namespace forecast {
const std::map<int, std::string> kWeatherCallouts = {
    {0, "Ясное небо"},
    {1, "Почти ясно"},
    {2, "Облачно"},
    {3, "Облачно"},
    {45, "Туман"},
    {48, "Туман"},
    {51, "Морось"},
    {53, "Морось"},
    {55, "Морось"},
    {56, "Ледоморось"},
    {57, "Ледоморось"},
    {61, "Дождь"},
    {63, "Дождь"},
    {65, "Дождь"},
    {66, "Ледодождь"},
    {67, "Ледодождь"},
    {71, "Снег"},
    {73, "Снег"},
    {75, "Снег"},
    {77, "Град"},
    {80, "Ливень"},
    {81, "Ливень"},
    {82, "Ливень"},
    {85, "Снегопад"},
    {86, "Снегопад"},
    {95, "Гроза"},
    {96, "Гроза с градом"},
    {99, "Гроза с градом"}
};



const std::vector<std::string> kArts{
    R"(     .-.
    (   ).
   (___(__)
    *  *  *
  *  *  * )",
    R"(     .-.
    (   ).
   (___(__)
    !  !  !
  !  !  ! )",
    R"(   \   /
    .-.
 --(   )--
    `-`
   /   \)",
    R"(
_ - _ - _ -
 _ - _ - _
_ - _ - _ -)",
    "    .--.  \n"
    "  .-(    ).\n"
    " (___.__)__)\n",
    "  \\  /     \n"
    "_ /\"\".-.   \n"
    "  \\_(   ). \n"
    "  /(___(__)\n",
    R"(     .-.
    (   ).
   (___(__)
    `. `. `
  ` .` .` )",
    R"(     .-.
    (   ).
   (___(__)
    `..`..`
  `..`..` )",
    R"(     .-.
    (   ).
   (___(__)
    !..!..!
  !..!..! )",
    R"(     .-.
    (   ).
   (___(__)
    .......
  ....... )",
    R"(     .-.
    (   ).
   (___(__)
    !!!!!!!
  !!!!!!! )",
    R"(     .-.
    (   ).
   (___(__)
    *******
  ******* )",
    R"(     .-.
    (\ \).
   (_/_(\_)
    /    /
    \   /)",
    R"(     .-.
    (\  ).
   (_/_(__)
    /....
    \....)"
};

enum IconNames {
  snow,
  rain,
  sunny,
  haze,
  overcast,
  pochti_sunny,
  moros,
  ledmoros,
  ledrain,
  grad,
  silniy_rain,
  snowfall,
  groza,
  groza_led
};

const std::map<std::string, IconNames> kWeatherIcons = {
    {"Ясное небо", sunny},
    {"Почти ясно", pochti_sunny},
    {"Облачно", overcast},
    {"Туман", haze},
    {"Морось", moros},
    {"Ледоморось", ledmoros},
    {"Дождь", rain},
    {"Ледодождь", ledrain},
    {"Снег", snow},
    {"Град", grad},
    {"Ливень", silniy_rain},
    {"Снегопад", snowfall},
    {"Гроза", groza},
    {"Гроза с градом", groza_led}
};

class Forecast {
  time_t current_time{}; // unix time
  double temperature_true{}; // deg celsius
  double temperature_feel{}; // deg celsius
  double precipitation{}; //millimeters
  double visibility{}; // meters
  double evapotranspiration{}; // meters
  double wind_speed{}; // km/h
  double wind_direction{}; // deg
  int32_t weather_code{};
 public:
  Forecast() = default;
  Forecast(time_t current_time,
           double temperature_true,
           double temperature_feel,
           double precipitation,
           int32_t weather_code,
           double visibility,
           double evapotranspiration,
           double wind_speed,
           double wind_direction)
      : current_time(current_time),
        temperature_true(temperature_true),
        temperature_feel(temperature_feel),
        precipitation(precipitation),
        visibility(visibility),
        evapotranspiration(evapotranspiration),
        wind_speed(wind_speed),
        wind_direction(wind_direction),
        weather_code(weather_code) {}

  double GetTemperatureTrue() const {
      return temperature_true;
  }

  double GetTemperatureFeel() const {
      return temperature_feel;
  }

  double GetPrecipitation() const {
      return precipitation;
  }

  double GetVisibility() const {
      return visibility;
  }

  double GetEvapotranspiration() const {
      return evapotranspiration;
  }

  double GetWindSpeed() const {
      return wind_speed;
  }

  double GetWindDirection() const {
      return wind_direction;
  }

  time_t GetCurTime() const {
      return current_time;
  }

  std::string GetNameFromWeatherCode() const {
      return kWeatherCallouts.find(weather_code)->second;
  }

  forecast::IconNames GetIconFromWeatherCode() const {
      return kWeatherIcons.find(GetNameFromWeatherCode())->second;
  }
};
}

namespace forecast::coordinates {
string CreateLinkForApiRequest(const string& city) {
    string proper_name;
    for (const auto& kItem : city) {
        if (std::isspace(kItem)) {
            proper_name += "%20";
        } else {
            proper_name += kItem;
        }
    }
    return "https://api.api-ninjas.com/v1/city"
           "?"
           "name=" + proper_name;
}

string GetApiAnswer(const string& city, const string& ApiKey) {
    cpr::Header headers{{"X-Api-Key", ApiKey}};
    auto response = cpr::Get(cpr::Url{CreateLinkForApiRequest(city)}, headers);

    if (response.status_code != 200 || response.text == "[]") {
        return "";
    }
    return response.text;
}
} // coordinates

namespace forecast::weather {
const int32_t kMaxDaysFromApi = 15;

string GetStartDate() {
    return to_iso_extended_string(boost::posix_time::second_clock::local_time().date());
} //"YYYY-MM-DD";
string GetEndDate() {
    return to_iso_extended_string(boost::gregorian::day_clock::local_day() + boost::gregorian::days(kMaxDaysFromApi));
}

string CreateLinkForApiRequest(const string& latitude, const string& longitude) {
    return "https://api.open-meteo.com/v1/forecast?"
           "latitude=" + latitude +
        "&"
        "longitude=" + longitude +
        "&"
        "hourly=temperature_2m,apparent_temperature,precipitation,weathercode,visibility,evapotranspiration,windspeed_80m,winddirection_80m"
        "&"
        "timeformat=unixtime"
        "&"
        "forecast_days=" + std::to_string(kMaxDaysFromApi) +
        "&"
        "GetStartDate=" + GetStartDate() +
        "&"
        "GetEndDate=" + GetEndDate() +
        "&"
        "timezone=Europe%2FMoscow";
}

string GetApiAnswer(const string& latitude, const string& longitude) {
    cpr::Response response = cpr::Get(cpr::Url{CreateLinkForApiRequest(latitude, longitude)});

    if (response.status_code != 200 || response.text == "[]") {
        return "";
    }
    return response.text;
}
} // weather

#endif