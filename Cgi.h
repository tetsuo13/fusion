/**
 * Fusion. Handles getting/setting of post, get, and environment variables.
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

#ifndef BLOG_CGI_CGI_H__
#define BLOG_CGI_CGI_H__

#include <string>
using std::string;

#include <map>
using std::map;

namespace blog_cgi
{

class Cgi
{
public:
    //Cgi(char* envp[]);
    Cgi(void);
    ~Cgi(void);
    void parse_post(void);
    void parse_get(void);
    void clear_post(void);
    void print_post_vars(void) const;

    // Public get functions.
    const char* get_env(const char* lpszKey) const;
    const char* get_post(const char* lpszKey, bool bSanitize = false);
    const char* get_get(const char* lpszKey) const;

    // Public set functions.
    void set_post(const char* lpszKey, const char* lpszValue);

    // Text manipulation functions.
    string trim(const string& source, const char* lpszDelimiters = " \t\r\n") const;
    void sanitize_text(string& text);
    void replace_character(string& haystack, const char* lpszNeedle, const char* lpszReplace);

private:
    // POST, GET, and environment variables will be stored int this key=>value
    // map object type.
    typedef map <string, string> mapStrType;

    // The entire POST string, in key=>value pairs delimited by ampersands.
    string m_post_data;

    // All key=>value pairs that were extracted from POST and GET.
    mapStrType m_post_vars;
    mapStrType m_get_vars;

    // All available environment variables.
    mapStrType m_env_vars;

    void stripKeyValue(string data, string& strKey, string& strValue);
    void uncgi(string data_string, mapStrType& var_map);
    void decode(string& data);
    void get_env_vars(char* envp[]);
    const char* get_map_val(const mapStrType& map_vars, const char* lpszKey) const;
};

} // namespace blog_cgi

#endif // #ifndef BLOG_CGI_CGI_H__
