#ifndef WEATHER_LIB_CITY_H_
#define WEATHER_LIB_CITY_H_

#include "Forecast.h"

using std::string;

namespace forecast::city {
class CityInfo {
  std::string city_name_;
  string latitude_;
  string longitude_;
  Forecast* current_hour_ = nullptr;
  std::vector<Forecast> hours_for_city_;

 public:
  const int32_t kHoursInDay = 24;
  const int32_t kHoursMorning = 8;
  const int32_t kHoursDay = 14;
  const int32_t kHoursEvening = 20;
  const int32_t kHoursNight = 26;

  const string& GetLatitude() const {
      return latitude_;
  }
  const string& GetLongitude() const {
      return longitude_;
  }
  Forecast* GetCurrentHour() const {
      return current_hour_;
  }
  const string& GetCityName() const {
      return city_name_;
  }
  std::vector<Forecast*> GetDayInfo(size_t day) {
      std::vector<Forecast*> ans(4, nullptr);
      if (hours_for_city_.size() <= day * kHoursInDay + kHoursNight) {
          return ans;
      }
      ans[0] = &hours_for_city_[day * kHoursInDay + kHoursMorning];
      ans[1] = &hours_for_city_[day * kHoursInDay + kHoursDay];
      ans[2] = &hours_for_city_[day * kHoursInDay + kHoursEvening];
      ans[3] = &hours_for_city_[day * kHoursInDay + kHoursNight];
      return ans;
  }

  std::string GetDayAndWeekday(size_t day) const {
      if (hours_for_city_.empty()) {
          return "ERR";
      }
      std::string posix_time_zone_string = "UTC+03:00:00";
      boost::posix_time::ptime pt = boost::posix_time::from_time_t(hours_for_city_[day * kHoursInDay].GetCurTime());
      boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(posix_time_zone_string));
      boost::local_time::local_date_time dt_with_zone(pt, zone);

      return dt_with_zone.local_time().date().day_of_week().as_long_string() + string(" ")
          + std::to_string(dt_with_zone.local_time().date().year_month_day().day.as_number()) + " "
          + dt_with_zone.local_time().date().year_month_day().month.as_long_string();
  }

  void UpdateWeatherInfo() {
      auto answer = weather::GetApiAnswer(latitude_, longitude_);
      if (answer.empty()) {
          return;
      }
      nlohmann::json data = nlohmann::json::parse(answer);

      nlohmann::json hourly_data = data["hourly"];
      std::vector<int> time = hourly_data["time"];
      std::vector<double> temperature_2m = hourly_data["temperature_2m"];
      std::vector<double> apparent_temperature = hourly_data["apparent_temperature"];
      std::vector<double> precipitation = hourly_data["precipitation"];
      std::vector<int> weathercode = hourly_data["weathercode"];
      std::vector<double> visibility = hourly_data["visibility"];
      std::vector<double> evapotranspiration = hourly_data["evapotranspiration"];
      std::vector<double> windspeed_80m = hourly_data["windspeed_80m"];
      std::vector<double> winddirection_80m = hourly_data["winddirection_80m"];
      std::vector<Forecast> hrs;
      auto id = 0;
      auto diff = INT64_MAX;
      auto now =
          std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

      for (int32_t i = 0; i < time.size(); ++i) {
          hrs.emplace_back(time[i],
                           temperature_2m[i],
                           apparent_temperature[i],
                           precipitation[i],
                           weathercode[i],
                           visibility[i],
                           evapotranspiration[i],
                           windspeed_80m[i],
                           winddirection_80m[i]
          );
          if (diff > abs(now - time[i])) {
              id = i;
              diff = abs(now - time[i]);
          }
      }
      hours_for_city_ = std::move(hrs);
      current_hour_ = &hours_for_city_[id];
  }

  void SetCoordinates(const string& ApiKey) {
      auto answer = forecast::coordinates::GetApiAnswer(city_name_, ApiKey);
      if (answer.empty()) {
          return;
      }
      nlohmann::json data = nlohmann::json::parse(answer);
      latitude_ = std::to_string(float(data[0]["latitude"]));
      longitude_ = std::to_string(float(data[0]["longitude"]));
      city_name_ = (string) data[0]["name"] + "(" + city_name_ + ")";
  }

  CityInfo(const string& name, const string& ApiKey) : city_name_(name) {
      SetCoordinates(ApiKey);
      if (!latitude_.empty())
          UpdateWeatherInfo();
  }
};
std::vector<CityInfo> CreateCitiesFromWeb(const std::vector<string>& input, const string& ApiKey) {
    std::vector<CityInfo> ans;
    for (const string& name : input) {
        CityInfo x(name, ApiKey);
        if (!x.GetLatitude().empty())
            ans.push_back(std::move(x));
    }

    return ans;
}
} // city
#endif