// Arduino Sunrise Library
// https://github.com/JChristensen/JC_Sunrise
// Copyright (C) 2021 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino library to calculate sunrise and sunset times.

#ifndef JC_SUNRISE_H_INCLUDED
#define JC_SUNRISE_H_INCLUDED
#include <Arduino.h>
#include <TimeLib.h>    // https://github.com/PaulStoffregen/Time

class JC_Sunrise
{
    public:
        JC_Sunrise(float lat, float lon, float zenith)
        : m_lat{lat}, m_lon{lon}, m_zenith{zenith} {}

        void calculate(time_t t, int utcOffset,
                       int &sunriseOut, int &sunsetOut);

        void calculate(time_t t, int utcOffset,
                       time_t &sunriseOut, time_t &sunsetOut);

        static constexpr float
            officialZenith {90.83333},
            civilZenith {96.0},
            nauticalZenith {102.0},
            astronomicalZenith {108.0},
            pi {3.141593};

    private:
        float m_lat;
        float m_lon;
        float m_zenith;
        void calcSunset(int doy, bool sunset, float utcOffset,
             uint8_t &hourOut, uint8_t &minutesOut);
        int ordinalDate(time_t t);
        bool isLeap(time_t t);
        float AdjustTo360(float i);
        float AdjustTo24(float i);
        float deg2rad(float degrees);
        float rad2deg(float radians);
};

#endif
