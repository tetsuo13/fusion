/**
 * Fusion. Implementation of the Call class.
 *
 * Copyright (C) 2005 Andrei Nicholson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Cal.h"

namespace an
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Cal::Cal(unsigned short year, unsigned short month) :
    m_year(year),
    m_month(month)
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Cal::~Cal(void)
{
}

// ----------------------------------------------------------------------------
// Calculates the day of the week that the first day of the given month and year
// occurs on. Uses Zeller's Congruence algorithm (NIST).
//
// @return - 0 = Sunday, 6 = Saturday.
// ----------------------------------------------------------------------------
unsigned short Cal::FirstDayOfMonth(void)
{
    // Used the Gregorian calendar algorithm found at Wikipedia's web site.

    const unsigned short t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    unsigned short year = m_year;

    if (m_month < 3)
    {
        year -= 1;
    }

    return (year + (year/4) - (year/100) + (year/400) + t[m_month-1] + 1) % 7;

/*
    // The remainder of this function is commented out as it failed for February
    // 2006.

    // Number of centuries, 18 for 1895.
    int centuries = m_year / 100;

    // Number of years through century, 95 for 1895.
    int year_century = m_year % 100;

    // The month of the year, with March having value 1, April 2, ..., December
    // 10, with January and February being counted as months 11 and 12 of the
    // preceeding year (in which case, subtract 1 from year_century).
    int month_year = (m_month + 10) % 12;

    if (m_month < 3)
    {
        if (year_century == 0)
        {
            year_century = 99;
            centuries -= 1;
        }
        else
        {
            year_century -= 1;
        }
    }

    float g = (1 + (floor((((13*month_year)-1)/5)) + year_century + (floor(year_century/4)) + (floor(centuries/4)) - (2*centuries)));

    // Cast result to int to take modulus.
    short day = static_cast <short>(g) % 7;

    if (day < 0)
    {
        day += 7;
    }

    return day;
*/
}

// ----------------------------------------------------------------------------
// Calculates the number of days in the given month.
// ----------------------------------------------------------------------------
unsigned short Cal::HowManyDays(void)
{
    // If we're currently looking at February and this is a leap year, 29.
    if ((m_month == 2) && LeapYear())
    {
        return 29;
    }

    // The number of days in each month of a non-leap year.
    const unsigned short days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    return days_in_month[m_month - 1];
}

// ----------------------------------------------------------------------------
// For debugging purposes. Will output the calendar, though not very pretty-like.
// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& ostr, Cal& CCal)
{
    unsigned short i;
    unsigned short start_day = CCal.FirstDayOfMonth();
    unsigned short end_day = CCal.HowManyDays();
    unsigned short day_of_month = 1;

    ostr << "S  M  T  W  T  F  S\n\n";

    for (i = 0; i < start_day; ++i)
    {
        ostr << "   ";
    }

    for (i = start_day; i < (start_day + end_day); ++i)
    {
        if (i % 7 == 0)
        {
            ostr << "\n";
        }

        ostr << day_of_month;

        if (day_of_month < 10)
        {
            ostr << "  ";
        }
        else
        {
            ostr << " ";
        }

        day_of_month++;
    }

    return ostr;
}

} // namespace an
