/**
 * Fusion. Basic HTML building blocks as well as output-oriented functions.
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

#ifndef BLOG_LAYOUT_LAYOUT_H__
#define BLOG_LAYOUT_LAYOUT_H__

#include <string>
#include <pqxx/pqxx>

#include "common.h"

namespace blog_layout
{

enum date_formats
{
    DATE_HEADER,
    ARCHIVE_MONTH,
    COMMENT,
    CALENDAR,
    ARCHIVE_COMMENT,
    ENTRY_TIME
};

class Layout
{
public:
    Layout(void);
    ~Layout(void);
    void init_i18n(void);

    void header(const char* lpszString, bool bSend = false) const;

    void print_page_header(const char* title) const;
    void print_page_footer(void) const;

    void print_sidebar(pqxx::connection& db, bool bAdmin = false);

    void print_main_column_begin(void) const;
    void print_main_column_end(void) const;

    const char* print_input(const std::string strType, const std::string strName,
                            const std::string strValue, const char* lpszSize = "0",
                            const char* lpszMaxLength = "0",
                            const std::string strSelected = "",
                            const char* extra = "");

    void fetch_settings(pqxx::connection& db);

    const char* format_date(const std::string sql_timestamp, const char* format);
    const char* format_date(const std::string sql_timestamp, unsigned short int format);

    const bool get_display_user_email(void) const;

private:
    // Each of the date formats has to have two versions: US and JP. This is
    // needed because strftime(), when given the same time format, does not
    // rearrange the order of the date parts. In Japanese, the year comes first.
    //
    // The entry date.
    const char* m_df_date_header;
    const char* m_df_date_header_jp;

    // Monthly archive date.
    const char* m_df_m_archive;
    const char* m_df_m_archive_jp;

    // Comment archive date.
    const char* m_df_c_archive;
    const char* m_df_c_archive_jp;

    // Comment time.
    const char* m_df_comment;
    const char* m_df_comment_jp;

    // Calendar date.
    const char* m_df_calendar;
    const char* m_df_calendar_jp;

    // Entry post time, shown in the "Posted by Andre at ~" line.
    const char* m_df_entry_time;
    const char* m_df_entry_time_jp;

    // Whether or not to switch to Japanese-style date formats. This is set in
    // init_i18n().
    bool m_i18n_jp;

    // This will be set to 'en_US' by default. If init_i18n() is called, the value
    // may change.
    std::string m_accept_lang;

    // Whether to display the supplied -- if supplied -- email address of
    // comments to entries.
    bool m_display_entry_comment_email;

    typedef struct attributes
    {
        std::string blog_name;
        std::string profile_location;
        std::string profile_comment;
    };

    attributes blog_attributes;

    void print_html_meta(const char* name, const char* value,
                         const char* http) const;
    void print_html_link(const char* relation, const char* href, const char* type,
                         const char* title, const char* extra) const;

    void print_feed_img(const char* feedType, const char* feedName) const;
    void print_sidebar_profile(const char* base_url) const;
    void print_sidebar_calendar(pqxx::connection& db);
    void print_sidebar_recent_posts(const char* base_url, pqxx::connection& db);
    void print_sidebar_recent_comments(const char* base_url, pqxx::connection& db);
    void print_sidebar_archive(const char* base_url, pqxx::connection& db);
    void print_sidebar_archive_month(const char* base_url, const char* mid,
                                     const char* timestamp, unsigned short monthCount);
    void print_sidebar_qr(void);

    std::string create_date(const char* timestamp, const char* format);

    // ------------------------------------------------------------------------
    // @arg date_format - A value from the date_formats enum.
    // ------------------------------------------------------------------------
    inline const char* flip_date(unsigned short date_format)
    {
        switch (date_format)
        {
        case DATE_HEADER:
            return ((m_i18n_jp) ? m_df_date_header_jp : m_df_date_header);
            break;

        case ARCHIVE_MONTH:
            return ((m_i18n_jp) ? m_df_m_archive_jp : m_df_m_archive);
            break;

        case ARCHIVE_COMMENT:
            return ((m_i18n_jp) ? m_df_c_archive_jp : m_df_c_archive);

        case COMMENT:
            return ((m_i18n_jp) ? m_df_comment_jp : m_df_comment);
            break;

        case CALENDAR:
            return ((m_i18n_jp) ? m_df_calendar_jp : m_df_calendar);
            break;

        case ENTRY_TIME:
            return ((m_i18n_jp) ? m_df_entry_time_jp : m_df_entry_time);
            break;
        }

        // This should never happen.
        return "";
    }
};

} // namespace blog_layout

#endif // #ifndef BLOG_LAYOUT_LAYOUT_H__
