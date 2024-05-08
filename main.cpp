//
// Created by al3v3r on 28.03.2023.
//
#include "lib/ConsoleInterface.cpp"
#include "lib/Forecast/City.h"

#include <thread>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>

const std::string kConfigArgName = "--config=";
std::mutex mt;

struct Config {
  std::vector<std::string> cities;
  size_t frequency{};
  uint8_t days{};
  std::string api_key;
  Config() = default;
  Config(const std::vector<std::string>& cities, size_t frequency, uint8_t days, const std::string& key)
      : cities(cities), frequency(frequency), days(days), api_key(key) {}

};

Config ParseArgs(int argc, char** argv) {
  for (size_t i = 1; i < argc; ++i) {
    std::string_view input{argv[i]};
    if (input.substr(0, kConfigArgName.size()) == kConfigArgName) {
      std::string filename = std::string(input.substr(kConfigArgName.size()));
      std::ifstream config_file(filename);
      if (config_file.is_open()) {
        try {
          nlohmann::json config;
          config_file >> config;
          return {config["cities"], config["frequency"], config["days"], config["api_key"]};
        } catch (...) {}
      }
    }
  }

    exit(-1);
}

void UpdateWeather(console::ConsoleInterface& console_interface,
                   const Config& config) {
  while (console::ConsoleInterface::active) {
    std::this_thread::sleep_for(std::chrono::milliseconds (config.frequency));
    auto x = forecast::city::CreateCitiesFromWeb(config.cities, config.api_key);
    if (!x.empty()) {
        std::unique_lock lc(mt);
        console_interface.SetCities(forecast::city::CreateCitiesFromWeb(config.cities, config.api_key));
    }
  }
}//

int main(int argc, char** argv) {
  Config config = ParseArgs(argc, argv);

  console::ConsoleInterface console_interface(config.days);
  console_interface.SetCities(forecast::city::CreateCitiesFromWeb(config.cities, config.api_key));

  std::thread drawer(&console::ConsoleInterface::Draw, &console_interface);
  std::thread updater(UpdateWeather, std::ref(console_interface), config);
  updater.detach();
  drawer.join();

  return 0;
}
