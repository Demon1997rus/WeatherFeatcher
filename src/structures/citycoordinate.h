#pragma once

#include <string>

#include <boost/move/move.hpp>
#include <boost/swap.hpp>

class CityCoordinate {
private:
    BOOST_COPYABLE_AND_MOVABLE(CityCoordinate)

public:
    CityCoordinate() = default;

    CityCoordinate(BOOST_COPY_ASSIGN_REF(CityCoordinate) other);

    CityCoordinate(BOOST_RV_REF(CityCoordinate) other) BOOST_NOEXCEPT;

    CityCoordinate &operator=(BOOST_COPY_ASSIGN_REF(CityCoordinate) other);

    CityCoordinate &operator=(BOOST_RV_REF(CityCoordinate) other) BOOST_NOEXCEPT;

    void swap(CityCoordinate &rhs) BOOST_NOEXCEPT;

public:
    void setCity(BOOST_COPY_ASSIGN_REF(std::string) city);
    void setCity(BOOST_RV_REF(std::string) city) BOOST_NOEXCEPT;
    void setLatitude(const double latitude);
    void setLongitude(const double longitude);

    std::string &getCity();
    BOOST_COPY_ASSIGN_REF(std::string) getCity() const;
    double getLatitude() const;
    double getLongitude() const;

private:
    std::string city;
    double latitude;
    double longitude;
};
