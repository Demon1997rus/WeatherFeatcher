#include "citycoordinate.h"

#include <utility>

void CityCoordinate::setCity(const std::string &city)
{
    this->city = city;
}

void CityCoordinate::setCity(std::string &&city) noexcept
{
    this->city = std::move(city);
}

void CityCoordinate::setLatitude(const double latitude)
{
    this->latitude = latitude;
}

void CityCoordinate::setLongitude(const double longitude)
{
    this->longitude = longitude;
}

const std::string &CityCoordinate::getCity() const
{
    return this->city;
}

double CityCoordinate::getLatitude() const
{
    return this->latitude;
}

double CityCoordinate::getLongitude() const
{
    return this->longitude;
}
