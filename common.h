/**
 * Fusion. Common functions that just wouldn't fit into a class.
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

#ifndef BLOG_LAYOUT_COMMON_H__
#define BLOG_LAYOUT_COMMON_H__

#include <libintl.h>
#include <locale>

namespace blog_layout
{

static const char* db_dsn = "host=localhost dbname=mydatabase user=myuser password=mypass";

static const char* base_url = "http://www.example.com/blog";

// ----------------------------------------------------------------------------
// Convenient wrapping function to gettext().
//
// @param lpszString - String to pass to gettext().
// @return gettext() output.
// ----------------------------------------------------------------------------
inline const char* _(const char* lpszString)
{
    return gettext(lpszString);
}

} // namespace blog_layout

#endif // #ifndef BLOG_LAYOUT_COMMON_H__
