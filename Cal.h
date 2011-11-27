/**
 * Fusion. Creates a calendar of a given year and month. Uses Zeller's
 * algorithm (from NIST) to calculate. Zeller's algorithm is very specifically
 * tied to the Gregorian calendar -- it will not work for years before 1582.
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

#ifndef AN_CAL__
#define AN_CAL__

#include <iostream>

namespace an
{

class Cal
{
    friend std::ostream& operator<<(std::ostream& ostr, Cal& cal);

public:
    Cal(unsigned short year, unsigned short month);
    ~Cal(void);
    unsigned short FirstDayOfMonth(void);
    unsigned short HowManyDays(void);

private:
    // The corresponding year/month to generate.
    unsigned short m_year;
    unsigned short m_month;

    inline bool LeapYear(void)
    {
        if ((m_year % 400 == 0) || ((m_year % 4 == 0) && (m_year % 100 != 0)))
        {
            return true;
        }

        return false;
    }
};

} // namespace an

#endif // #ifndef AN_CAL__
