// Arduino Sunrise Library
// https://github.com/JChristensen/JC_Sunrise
// Copyright (C) 2021 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino library to calculate sunrise and sunset times.

#include <JC_Sunrise.h>

// calculate sunrise and sunset as single integers, i.e. hhmm,
// given epoch time and utcOffset in minutes.
void JC_Sunrise::calculate(time_t t, int utcOffset,
                           int &sunriseOut, int &sunsetOut)
{
    uint8_t hour, minute;
    int ord = ordinalDate(t);
    float fOffset = utcOffset / 60.0;
    calcSunset(ord, false, fOffset, hour, minute);
    sunriseOut = 100 * hour + minute;
    calcSunset(ord, true,  fOffset, hour, minute);
    sunsetOut  = 100 * hour + minute;
}

// calculate sunrise and sunset as time_t values,
// given epoch time and utcOffset in minutes.
void JC_Sunrise::calculate(time_t t, int utcOffset,
                           time_t &sunriseOut, time_t &sunsetOut)
{
    uint8_t hour, minute;
    tmElements_t tm;
    breakTime(t, tm);
    int ord = ordinalDate(t);
    float fOffset = utcOffset / 60.0;

    calcSunset(ord, false, fOffset, hour, minute);
    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = 0;
    sunriseOut = makeTime(tm);

    calcSunset(ord, true,  fOffset, hour, minute);
    tm.Hour = hour;
    tm.Minute = minute;
    sunsetOut = makeTime(tm);
}

/*----------------------------------------------------------------------*
 * This is an implementation of the Sunrise/Sunset Algorithm found at   *
 * http://williams.best.vwh.net/sunrise_sunset_algorithm.htm            *
 * from the Almanac for Computers, 1990                                 *
 * Published by Nautical Almanac Office                                 *
 * Washington, DC 20392                                                 *
 * Implemented by Chris Snyder                                          *
 * Modified 09Dec2011 by Jack Christensen                               *
 *  - Improved rounding of the returned hour and minute values          *
 *    (e.g. would sometimes return 16h60m rather than 17h0m)            *
 *  - Replaced dayNumber() function with ordinalDate() and isLeap().    *
 *    (dayNumber returned zero as the first day for non-leap years.)    *
 *                                                                      *
 *    These changes resulted in better agreement with the US Naval      *
 *    Observatory calculations,                                         *
 *    http://aa.usno.navy.mil/data/docs/RS_OneYear.php                  *
 *                                                                      *
 *    For 2011, for my locale, W083°37', N42°56', all sunrise and       *
 *    sunset times agreed within one minute. 12 sunrise times were one  *
 *    minute later than the USNO time, and 18 earlier. 19 sunset times  *
 *    were one minute later and 2 were earlier.                         *
 *----------------------------------------------------------------------*/

// Function name: calcSunset
// Parameters:
// doy: The day of the year to calculate sunset/rise for
// lat: The latitude of the location to calculate sunset/rise for
// lon: The longitude of the location to calculate sunset/rise for
// sunset: true to calculate sunset, false to calculate sunrise
// utcOffset: difference in hours from UTC
// zenith: Sun's zenith for sunrise/sunset
//         offical      = 90 degrees 50'  (90.8333)
//         civil        = 96 degrees
//         nautical     = 102 degrees
//         astronomical = 108 degrees
// hourOut, minutesOut: Return values, time of sunrise or sunset
//
// Note: longitude is positive for East and negative for West
//       latitude is positive for North and negative for south

void JC_Sunrise::calcSunset(int doy, bool sunset, float utcOffset,
                            uint8_t &hourOut, uint8_t &minutesOut)
{
    hourOut = minutesOut = 0;

    // Convert the longitude to hour value and calculate an approximate time.
    float lonhour = (m_lon / 15);

    float t;
    if (sunset)
        t = doy + ((18 - lonhour) / 24);
    else
        t = doy + ((6 - lonhour) / 24);

    // Calculate the Sun's mean anomaly
    float m = (0.9856 * t) - 3.289;

    // Calculate the Sun's true longitude
    float sinm = sin(deg2rad(m));
    float sin2m = sin(2 * deg2rad(m));
    float l= AdjustTo360 (m + (1.916 * sinm) + (0.02 * sin2m) + 282.634);

    // Calculate the Sun's right ascension(RA)
    float tanl = 0.91764 * tan(deg2rad(l));
    float ra = AdjustTo360 (rad2deg(atan(tanl)));

    // Putting the RA value into the same quadrant as L
    float lq = (floor(l / 90)) * 90;
    float raq = (floor(ra / 90)) * 90;
    ra = ra + (lq - raq);

    // Convert RA values to hours
    ra /= 15;

    // Calculate the Sun's declination
    float sindec = 0.39782 * sin(deg2rad(l));
    float cosdec = cos(asin(sindec));

    // Calculate the Sun's local hour angle
    float cosh = (cos(deg2rad(m_zenith)) - (sindec * sin(deg2rad(m_lat))))
        / (cosdec * cos(deg2rad(m_lat)));

    // if cosH > 1 the sun never rises on this date at this location
    // if cosH < -1 the sun never sets on this date at this location
    if (cosh >  1)
        return;
    else if (cosh < -1)
        return;

    // Finish calculating H and convert into hours
    float h;
    if (sunset)
        h = rad2deg(acos(cosh));
    else
        h = 360 - rad2deg(acos(cosh));

    h /= 15;

    // Calculate local mean time of rising/setting
    t = h + ra - (0.06571 * t) - 6.622;

    // Adjust back to UTC
    float ut = AdjustTo24(t - lonhour);
    // Adjust for current time zone
    // round up to the next minute by adding 30 seconds (0.00833333 hour) -- jc
    ut = AdjustTo24(ut + utcOffset) + 0.00833333;
    // was: ut = AdjustTo24(ut + utcOffset);

    hourOut = floor(ut);
    // rounded above, so letting the float-to-int assignment truncate is OK -- jc
    // was: minutesOut = round(60 * (ut - hour));
    minutesOut = 60.0 * (ut - hourOut);
}

// Leap years are those divisible by 4, but not those divisible by 100,
// except that those divisible by 400 *are* leap years.
// See Kernighan & Ritchie, 2nd edition, section 2.5.
bool JC_Sunrise::isLeap(time_t t)
{
    int y = year(t);
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

// Return ordinal day of year for the given time_t
int JC_Sunrise::ordinalDate(time_t t)
{
    int m = month(t);
    int d = day(t);

    if (m == 1)
        return d;
    else if (m == 2)
        return d + 31;
    else {
        int n = floor(30.6 * (m + 1)) + d - 122;
        return n + (isLeap(t) ? 60 : 59);
    }
}

float JC_Sunrise::AdjustTo360(float i)
{
    if (i > 360.0)
        i -= 360.0;
    else if (i < 0.0)
        i += 360.0;
    return i;
}

float JC_Sunrise::AdjustTo24(float i)
{
    if (i > 24.0)
        i -= 24.0;
    else if (i < 0.0)
        i += 24.0;
    return i;
}

float JC_Sunrise::deg2rad(float degrees)
{
    return degrees * pi / 180.0;
}

float JC_Sunrise::rad2deg(float radians)
{
    return radians / (pi / 180.0);
}
