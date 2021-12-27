# Arduino Sunrise Library
https://github.com/JChristensen/JC_Sunrise  
README file  

## License
Arduino Sunrise Library Copyright (C) 2021 Jack Christensen GNU GPL v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/gpl.html>

## Introduction
Library to calculate sunrise and sunset times, for a given latitude, longitude, and zenith of the sun.
### Prerequisite Libraries
- TimeLib.h (<https://github.com/PaulStoffregen/Time>)

### Example Sketch
The following example sketch is included with the **JC_Sunrise** library:

- **PrintListing**: Prints a listing of sunrise and sunset times for a given range of years.

## Constructor

### JC_Sunrise(lat, lon, zenith)
##### Description
The constructor defines a JC_Sunrise object for a particular location.
##### Syntax
`JC_Sunrise(lat, lon, zenith);`
##### Required parameters
**lat:** Latitude *(float)*  
**lon:** Longitude *(float)*  
**zenith:** Sun's zenith for sunrise/sunset *(float)*  

Several constants are available that can be used for the zenith parameter:
```c++
JC_Sunrise::officialZenith {90.83333}  
JC_Sunrise::civilZenith {96.0}  
JC_Sunrise::nauticalZenith {102.0}  
JC_Sunrise::astronomicalZenith {108.0}
```
##### Example
```c++
constexpr float myLat {45.8171}, myLon {-84.7278};
JC_Sunrise sun {myLat, myLon, JC_Sunrise::officialZenith};

```

## Library Functions

### void calculate(time_t t, int utcOffset, int &sunriseOut, int &sunsetOut);
##### Description
Calculates sunrise and sunset times as single integers of the form HHMM, for the day given by the local epoch time `t`.
##### Syntax
`calculate(t, utcOffset, sunriseOut, sunsetOut);`
##### Input Parameters
**t:** Local epoch time *(time_t)*  
**utcOffset:** UTC offset in minutes *(int)*  
##### Output Parameters
**sunriseOut:** Time of sunrise for the given day, HHMM *(int)*  
**sunsetOut:** Time of sunset for the given day, HHMM *(int)*
##### Returns
None.
##### Example
```c++
constexpr float myLat {45.8171}, myLon {-84.7278};
JC_Sunrise sun {myLat, myLon, JC_Sunrise::officialZenith};

// 01Jan2022 08:00:00 EST0
int utcOffset {-300};
tmElements_t tm;
tm.Year = 2022 - 1970;
tm.Month = tm.Day = 1;
tm.Hour = 8; tm.Minute = tm.Second = 0;
time_t localTime = makeTime(tm);
int sunrise, sunset;
sun.calculate(localTime, utcOffset, sunrise, sunset);
Serial.print(sunrise);      // 820
Serial.print(' ');
Serial.println(sunset);     // 1705
```

### void calculate(time_t t, int utcOffset, time_t &sunriseOut, time_t &sunsetOut);
##### Description
As above, except the sunrise and sunset times are returned as `time_t` values.
##### Syntax
`calculate(t, utcOffset, sunriseOut, sunsetOut);`
##### Input Parameters
**t:** Local epoch time *(time_t)*  
**utcOffset:** UTC offset in minutes *(int)*  
##### Output Parameters
**sunriseOut:** Time of sunrise for the given day *(time_t)*  
**sunsetOut:** Time of sunset for the given day *(time_t)*
##### Returns
None.
##### Example
```c++
constexpr float myLat {45.8171}, myLon {-84.7278};
JC_Sunrise sun {myLat, myLon, JC_Sunrise::officialZenith};

// 01Jan2022 08:00:00 EST
int utcOffset {-300};
tmElements_t tm;
tm.Year = 2022 - 1970;
tm.Month = tm.Day = 1;
tm.Hour = 8; tm.Minute = tm.Second = 0;
time_t localTime = makeTime(tm);
time_t sunrise, sunset;
sun.calculate(localTime, utcOffset, sunrise, sunset);
Serial.print(hour(sunrise));        // 8
Serial.print(' ');
Serial.print(minute(sunrise));      // 20
Serial.print(' ');
Serial.print(hour(sunset));         // 17
Serial.print(' ');
Serial.println(minute(sunset));     // 5
```
