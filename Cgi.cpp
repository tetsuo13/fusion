/**
 * Fusion. Implementation of Cgi class.
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

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include "Cgi.h"


namespace blog_cgi
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Cgi::Cgi(void)
{
    // Is there any need for this? There's a getenv() functions already available.
    //get_env_vars(envp);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Cgi::~Cgi(void)
{
}

// ----------------------------------------------------------------------------
// Looks at all available environment variables and puts them into a map for
// key => value access.
//
// @param envp - The environment character array passed into main().
// ----------------------------------------------------------------------------
void Cgi::get_env_vars(char* envp[])
{
    string strKey, strValue;

    // Go through all available environment variables. All envp values are in the
    // form of "key=value". Pass this string out to be split and then insert the
    // key and values into a map element.
    for (unsigned short i = 0; envp[i] != NULL; ++i)
    {
        stripKeyValue(envp[i], strKey, strValue);

        m_env_vars.insert(mapStrType::value_type(strKey, strValue));
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::parse_post(void)
{
    getline(cin, m_post_data);

    // If there was an error in getline() or there is no POST data available,
    // don't continue processing.
    if (!m_post_data.length())
    {
        return;
    }

    uncgi(m_post_data, m_post_vars);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::parse_get(void)
{
    // Make sure there's GET variables to look at first.
    if (!getenv("QUERY_STRING"))
    {
        return;
    }

    uncgi(getenv("QUERY_STRING"), m_get_vars);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::clear_post(void)
{
    m_post_vars.clear();
}

// ----------------------------------------------------------------------------
// Finds the key=>value names from a given string.
//
// @param data - The string containing the two pairs.
// @param strKey - Address to a string to put key name into.
// @param strValue - Address to a string to put value into.
// ----------------------------------------------------------------------------
void Cgi::stripKeyValue(string data, string& strKey, string& strValue)
{
    string::size_type search = 0;

    // Find the first occurance of an equal sign.
    search = data.find_first_of("=");

    // Everything from the start of the string up to the equal character is the
    // key. Grab it.
    strKey = data.substr(0, search);

    // Grab everything after the equal sign. If no value is present, strValue
    // will be empty.
    strValue = data.substr(search+1);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::uncgi(string data_string, mapStrType& var_map)
{
    unsigned short ushLineLength = data_string.size();
    string strKey, strValue;
    string::size_type search = 0;

    while (ushLineLength > 0)
    {
        search = data_string.find_first_of("&");

        stripKeyValue(data_string.substr(0, search), strKey, strValue);

        // If the last find_first_of("&") couldn't find an ampersand, we're at
        // the of the POST data; or if there's a trailing ampersand and one more
        // character throws us out of bounds, we're at the end of POST data.
        if (search == string::npos || (search+1) == string::npos)
        {
            ushLineLength = 0;
        }
        else
        {
            data_string.erase(0, (search+1));

            ushLineLength = data_string.size();
        }

        decode(strValue);

        strValue = trim(strValue);

        var_map.insert(mapStrType::value_type(strKey, strValue));

        strKey.empty();
        strValue.empty();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::decode(string& data)
{
    string decoded_data;
    string::const_iterator iter = data.begin();

    while (iter != data.end())
    {
        if (*iter == '%')
        {
            if (++iter == data.end())
            {
                break;
            }

            int d = *iter;

            if (++iter == data.end())
            {
                break;
            }

            int e = *iter;

            if (d >= 'A')
            {
                d = (d & 0xdf) - 'A' + 10;
            }
            else
            {
                d -= '0';
            }

            if (e >= 'A')
            {
                e = (e & 0xdf) - 'A' + 10;
            }
            else
            {
                e -= '0';
            }

            //d = (d >= 'A') ? (d & 0xdf) - 'A' + 10 : d - '0';
            //e = (e >= 'A') ? (e & 0xdf) - 'A' + 10 : e - '0';

            if (d >= 0 && d < 16 && e >= 0 && e < 16)
            {
                decoded_data += static_cast <char>(16 * d + e);
            }
        }
        else if (*iter == '+')
        {
            decoded_data += ' ';
        }
        else if (*iter == '&')
        {
            decoded_data += '\n';
        }
        else
        {
            decoded_data += *iter;
        }

        iter++;
    }

    data = decoded_data;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::print_post_vars(void) const
{
    cout << "<pre>Post variables (" << m_post_vars.size() << "):" << endl;
    for (mapStrType::const_iterator iter = m_post_vars.begin(); iter != m_post_vars.end(); ++iter)
    {
        cout << iter->first << " => " << iter->second << endl;
    }
    cout << "</pre>" << endl;

/*
    cout << "<Pre>Env variables" << endl;
    for (mapStrType::const_iterator it = m_env_vars.begin(); it != m_env_vars.end(); ++it)
    {
        cout << it->first << " => " << it->second << endl;
    }
    cout << "</pre>" << endl;
*/
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const char* Cgi::get_env(const char* lpszKey) const
{
    return get_map_val(m_env_vars, lpszKey);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const char* Cgi::get_post(const char* lpszKey, bool bSanitize)
{
    string ret = get_map_val(m_post_vars, lpszKey);

    if (bSanitize)
    {
        sanitize_text(ret);
    }

    return ret.c_str();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const char* Cgi::get_get(const char* lpszKey) const
{
    return get_map_val(m_get_vars, lpszKey);
}

// ----------------------------------------------------------------------------
// Searches through a given map object for a key value matching lpszKey. If
// found, returns its associated value.
//
// @param map_vars - A map object.
// @param lpszKey - The needle to search for.
// ----------------------------------------------------------------------------
const char* Cgi::get_map_val(const mapStrType& map_vars, const char* lpszKey) const
{
    mapStrType::const_iterator iter = map_vars.find(lpszKey);

    if (iter != map_vars.end())
    {
        return (*iter).second.c_str();
    }

/*
    for (mapStrType::const_iterator iter = map_vars.begin(); iter != map_vars.end(); ++iter)
    {
        if (iter->first == lpszKey)
        {
            return iter->second.c_str();
        }
    }
*/

    return "";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Cgi::set_post(const char* lpszKey, const char* lpszValue)
{
    m_post_vars[lpszKey] = lpszValue;

/*
    mapStrType::interator iter = m_post_vars.find(lpszKey);

    if (iter != m_post_vars.end())
    {
        m_post_vars.insert(mapStrType::value_type(lpszKey, lpszValue));
    }
*/
}

// ----------------------------------------------------------------------------
// Removes any redundant whitespace from the beginning and end of a given string.
//
// @param source - The source text to parse. Will not be modified.
// @param lpszDelimiters - A set of delimiters to trim.
// @return A copy of source that has been trimmed.
// ----------------------------------------------------------------------------
string Cgi::trim(const string& source, const char* lpszDelimiters) const
{
    string result(source);
    string::size_type index = result.find_last_not_of(lpszDelimiters);

    if (index != string::npos)
    {
        result.erase(++index);
    }

    index = result.find_first_not_of(lpszDelimiters);

    if (index != string::npos)
    {
        result.erase(0, index);
    }
    else
    {
        result.erase();
    }

    return result;
}

// ----------------------------------------------------------------------------
// Converts special characters into HTML entities.
//
// @param heystack - The string to parse and manipulate.
// ----------------------------------------------------------------------------
void Cgi::sanitize_text(string& haystack)
{
    // Don't bother doing anything if the string is empty.
    if (!haystack.length())
    {
        return;
    }

    haystack = trim(haystack);

    replace_character(haystack, "&", "&amp;");
    replace_character(haystack, "'", "&#039;");
    replace_character(haystack, "\"", "&quot;");
    replace_character(haystack, "<", "&lt;");
    replace_character(haystack, ">", "&gt;");
}

// ----------------------------------------------------------------------------
// Iterates through a given text string and replaces all occurances of lpszNeedle
// with lpszReplace.
//
// @param haystack - The given text string.
// @param lpszNeedle - Text to search for.
// @param lpszReplace - Replacement for lpszNeedle.
// ----------------------------------------------------------------------------
void Cgi::replace_character(string& haystack, const char* lpszNeedle,
                            const char* lpszReplace)
{
    string::size_type index = haystack.find(lpszNeedle, 0);

    // Keep searching the haystack until there are no more needles.
    while (index != string::npos)
    {
        haystack.replace(index, static_cast <int>(strlen(lpszNeedle)), lpszReplace);

        // Advance the find base position the number of lpszNeedle characters
        // so that we won't have to search from the beginning again.
        index += static_cast <int>(strlen(lpszNeedle));

        index = haystack.find(lpszNeedle, index);
    }
}

} // namespace blog_cgi
