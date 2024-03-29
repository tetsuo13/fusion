/**
 * Fusion. Handles all form error checking statuses.
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

#ifndef FORM_INPUT_FORMINPUT_H__
#define FORM_INPUT_FORMINPUT_H__

#include <string>
using std::string;

#include <map>
using std::map;

namespace form_input
{

class FormInput
{
public:
    FormInput(const char* lpszErrorPrefix, const char* lpszErrorSuffix);
    ~FormInput(void);
    bool is_form_okay(void) const;
    void set_error(const char* lpszInputName, const char* lpszErrorString);
    const char* print_error(const char* lpszInputName) const;

private:
    typedef map <const char*, const char*> mapStrType;

    mapStrType m_form_inputs;
    bool m_bFormOkay;

    string m_ErrorPrefix;
    string m_ErrorSuffix;
};

} // namespace form_input

#endif // #ifndef FORM_INPUT_FORMINPUT_H__
