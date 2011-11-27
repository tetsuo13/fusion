/**
 * Fusion. Implementation of the Mail class.
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

#include <sstream>

#include "Mail.h"

namespace an
{

// Couldn't find a way to delcare this array in the header file, ran into problems
// when I declared a 'const char*' in the header and initialized it in the
// constructor here. Only way I could make this work was like this.
const char* Mail::m_priority_levels[5] =
{
    "Highest", "High", "Normal", "Low", "Lowest"
};


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Mail::Mail() :
    m_smtp_mailer_path("/opt/exim/bin/exim -ti"),
    m_charset("iso-8859-1")
{
    SetHeader("User-Agent", "fusion");
    SetHeader("X-Mailer", "fusion");

    SetPriority(3);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Mail::~Mail()
{
}

// ----------------------------------------------------------------------------
// Sets the priority level for the message. All that's needed is the level and
// it will set the appropriate headers.
//
// @param priority - The level to use. 3 is default.
// ----------------------------------------------------------------------------
void Mail::SetPriority(unsigned short int priority)
{
    std::ostringstream ossStream;
    ossStream << priority;

    std::string x_priority = ossStream.str();
    x_priority += " (";
    x_priority +=      m_priority_levels[priority - 1];
    x_priority += ")";

    SetHeader("X-Priority", x_priority.c_str());
    SetHeader("Importance", m_priority_levels[priority - 1]);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::SetHeader(const char* header, const char* value)
{
    m_headers += header;
    m_headers += ": ";
    m_headers += value;
    m_headers += "\r\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::SetFrom(const char* email, const char* name)
{
    SetFromTo("From", email, name);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::SetTo(const char* email, const char* name)
{
    SetFromTo("To", email, name);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::SetFromTo(const char* direction, const char* email, const char* name)
{
    std::string meta(email);

    // If a name is specified, wrap the email in angel brakets.
    if (static_cast <int>(strlen(name)))
    {
        meta  = name;
        meta += " <";
        meta += email;
        meta += ">";
    }

    SetHeader(direction, meta.c_str());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::SetSubject(const char* subject)
{
    SetHeader("Subject", subject);
}

// ----------------------------------------------------------------------------
// Gets the Content-Transfer-Encoding header text based on the charset of the
// email.
// ----------------------------------------------------------------------------
const char* Mail::GetContentTransferEncoding(void)
{
    // If m_charset == unicode then return 'base64'

    return "8bit";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::Build(void)
{
    SetHeader("MIME-Version", "1.0");

    // If there are no attachments...
    SetHeader("Content-Type", GenerateContentType());
    SetHeader("Content-Transfer-Encoding", GetContentTransferEncoding());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Mail::Send(void)
{
    Build();

    FILE* mailer = popen(m_smtp_mailer_path, "w");

    if (mailer)
    {
        fputs(m_headers.c_str(), mailer);
        fputs("\n", mailer);
        fputs(m_body.c_str(), mailer);

        pclose(mailer);
    }
}

} // namespace an
