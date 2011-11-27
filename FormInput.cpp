/**
 * Fusion. Implementation of FormInput class.
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

#include "FormInput.h"

namespace form_input
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
FormInput::FormInput(const char* lpszErrorPrefix, const char* lpszErrorSuffix) :
    m_bFormOkay(true),
    m_ErrorPrefix(lpszErrorPrefix),
    m_ErrorSuffix(lpszErrorSuffix)
{
/*
    // Set the form to initially true; no errors.
    m_bFormOkay = true;

    m_ErrorPrefix = lpszErrorPrefix;
    m_ErrorSuffix = lpszErrorSuffix;
*/
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
FormInput::~FormInput(void)
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool FormInput::is_form_okay(void) const
{
    return m_bFormOkay;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void FormInput::set_error(const char* lpszInputName, const char* lpszErrorString)
{
    m_form_inputs.insert(mapStrType::value_type(lpszInputName, lpszErrorString));

    m_bFormOkay = false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const char* FormInput::print_error(const char* lpszInputName) const
{
    for (mapStrType::const_iterator iter = m_form_inputs.begin(); iter != m_form_inputs.end(); ++iter)
    {
        if (strcmp(iter->first, lpszInputName) == 0)
        {
            string ret(m_ErrorPrefix);
            ret += iter->second;
            ret += m_ErrorSuffix;

            return ret.c_str();
            break;
        }
    }

    return "";
}

} // namespace form_input
