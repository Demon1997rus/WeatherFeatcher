#include "citycoordinate.h"

CityCoordinate::CityCoordinate(BOOST_COPY_ASSIGN_REF(CityCoordinate) other) :
    city(other.city), latitude(other.latitude), longitude(other.longitude)
{
}

CityCoordinate::CityCoordinate(BOOST_RV_REF(CityCoordinate) other) BOOST_NOEXCEPT
{
    this->swap(other);
}

CityCoordinate &CityCoordinate::operator=(BOOST_COPY_ASSIGN_REF(CityCoordinate) other)
{
    CityCoordinate temp(other);
    this->swap(temp);
    return *this;
}

CityCoordinate &CityCoordinate::operator=(BOOST_RV_REF(CityCoordinate) other) BOOST_NOEXCEPT
{
    CityCoordinate temp(boost::move(other));
    this->swap(temp);
    return *this;
}

void CityCoordinate::swap(CityCoordinate &rhs) BOOST_NOEXCEPT
{
    this->city.swap(rhs.city);
    boost::swap(this->latitude, rhs.latitude);
    boost::swap(this->longitude, rhs.longitude);
}

void CityCoordinate::setCity(BOOST_COPY_ASSIGN_REF(std::string) city)
{
    this->city = city;
}

void CityCoordinate::setCity(BOOST_RV_REF(std::string) city) BOOST_NOEXCEPT
{
    this->city = boost::move(city);
}

void CityCoordinate::setLatitude(const double latitude)
{
    this->latitude = latitude;
}

void CityCoordinate::setLongitude(const double longitude)
{
    this->longitude = longitude;
}

std::string &CityCoordinate::getCity()
{
    return this->city;
}

BOOST_COPY_ASSIGN_REF(std::string) CityCoordinate::getCity() const
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
