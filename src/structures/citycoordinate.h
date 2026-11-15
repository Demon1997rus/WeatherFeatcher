#pragma once

#include <string>

class CityCoordinate {
public:
    void setCity(const std::string &city);
    void setCity(std::string &&city) noexcept;
    void setLatitude(double latitude);
    void setLongitude(double longitude);

    const std::string &getCity() const;
    double getLatitude() const;
    double getLongitude() const;

private:
    std::string city;
    double latitude = 0.0;
    double longitude = 0.0;
};
