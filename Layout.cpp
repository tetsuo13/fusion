/**
 * Fusion. Implementation of the Layout class.
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
#include <ctime>

#include "Layout.h"
#include "Cal.h"


namespace blog_layout
{

// ----------------------------------------------------------------------------
// Using kanji in the string is cheating; should let strftime() take care of all
// of the localization. However, I couldn't get the correct year representation!
// ----------------------------------------------------------------------------
Layout::Layout(void) :
    m_df_date_header("%B %e, %Y"),
    m_df_date_header_jp("%x（%a）"),

    m_df_m_archive("%B %Y"),
    m_df_m_archive_jp("%Y年%m月"),

    m_df_c_archive("%m/%d"),
    m_df_c_archive_jp("%m月%d日"),

    m_df_comment("%B %e, %Y %l:%M %p"),
    m_df_comment_jp("%x（%a）%p%l:%M"),

    m_df_calendar("%B %Y"),
    m_df_calendar_jp("%Y年%m月"),

    m_df_entry_time("%l:%M %p"),
    m_df_entry_time_jp("%p%l:%M")
{
    m_accept_lang = "en_US.utf8";
    m_i18n_jp = false;

    m_display_entry_comment_email = false;

    //setenv("TZ", "JST-9", 1);
    setenv("TZ", "EST", 1);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
Layout::~Layout(void)
{
}

// ----------------------------------------------------------------------------
// Outputs a META tag.
// ----------------------------------------------------------------------------
void Layout::print_html_meta(const char* name, const char* value, const char* http) const
{
    std::cout << "<meta " << http << "=\"" << name << "\" content=\"" << value << "\" />" << std::endl;
}

// ----------------------------------------------------------------------------
// Outputs a LINK tag.
// ----------------------------------------------------------------------------
void Layout::print_html_link(const char* relation, const char* href, const char* type,
                             const char* title, const char* extra) const
{
    std::cout << "<link rel=\"" << relation << "\" ";

    if (strcmp(type, "") != 0)
    {
        std::cout << "type=\"" << type << "\" ";
    }

    if (strcmp(title, "") != 0)
    {
        std::cout << "title=\"" << title << "\" ";
    }

    if (strcmp(extra, "") != 0)
    {
        std::cout << extra << " ";
    }

    std::cout << "href=\"" << base_url;
    std::cout << href << "\" />" << std::endl;
}

// ----------------------------------------------------------------------------
// Outputs a header. Function intended to be called before a blank line feed.
// If this is the final header and the extra line feed is output, it will not
// exit application -- that is up to the calling function if it wants to.
//
// @param lpszString - The header string to output.
// @param bSend - If this is the final header to send, output an extra "\r\n".
// ----------------------------------------------------------------------------
void Layout::header(const char* lpszString, bool bSend) const
{
    std::cout << lpszString << std::endl;

    if (bSend)
    {
        std::cout << std::endl;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_page_header(const char* lpszTitle) const
{
/*
    header("HTTP/1.0 200 OK");
    header("HTTP/1.1 200 OK");
    header("Content-Type: text/html; charset=UTF-8");

    header("Expires: Sun, 13 Aug 1995 13:00:00 GMT");
    header("Last-Modifided: ".gmdate("D, d M Y H:i:s")." GMT");

    // HTTP/1.1
    header("Cache-Control: no-store, no-cache, must-revalidate");
    header("Cache-Control: post-check=0, pre-check=0", false);

    // HTTP/1.0
    header("Pragma: no-cache");
*/

    //std::cout << "HTTP/1.0 200\r\n";
    //std::cout << "HTTP/1.1 200\r\n";

    header("Content-type: text/html; charset=UTF-8", true);

    std::cout << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
              << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
    std::cout << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
    std::cout << "<head>" << std::endl;

    print_html_meta("content-type",  "text/html; charset=UTF-8",     "http-equiv");

    std::cout << "<title>" << blog_attributes.blog_name << lpszTitle << "</title>" << std::endl;

    print_html_meta("pragma",        "no-cache",                     "http-equiv");
    print_html_meta("cache-control", "no-cache",                     "http-equiv");
    print_html_meta("expires",       "Wed, 1 Jan 1997 05:00:00 GMT", "http-equiv");
    print_html_meta("author",        "Andrei Nicholson",             "name");
    print_html_meta("robots",        "index, follow",                "name");

    print_html_link("alternate", "feed.xml?type=atom", "application/atom+xml", "Atom 1.0 - fusion feed", "");
    print_html_link("alternate", "feed.xml?type=rss", "application/rss+xml", "RSS 2.0 - fusion feed", "");
    print_html_link("alternate", "feed.xml?type=rdf", "application/rdf+xml", "RDF 1.0 - fusion feed", "");
    print_html_link("stylesheet", "content.css", "text/css", "", "media=\"screen, projection\"");
    print_html_link("home", "", "", "Home", "");

    std::cout << "</head>" << std::endl;
    std::cout << "<body>" << std::endl;

    // Begin #content - Centers all content and provides edges for floated
    // columns.
    std::cout << "<div id=\"content\">" << std::endl;

    // Begin #header.
    std::cout << "<div id=\"header\"><div>" << std::endl;
    std::cout << "<h1 id=\"blog-title\"><a href=\"" << base_url << "\">" << blog_attributes.blog_name << "</a></h1>" << std::endl;

/*
    std::cout << "<div id=\"blog-page\">" << std::endl;
    std::cout << "<a href=\"#\" title=\"Toggle page font style between Sans-serif and Serif.\">A</a>" << std::endl;
    std::cout << "<a href=\"#\" title=\"Decrease font size.\">-</a>" << std::endl;
    std::cout << "<a href=\"#\" title=\"Increase font size.\">+</a>" << std::endl;
    std::cout << "</div>" << std::endl;
*/

    std::cout << "<p id=\"description\">It&#039;s not the mountain ahead that wears us out, but the grains of sand in our shoes.</p>" << std::endl;

    std::cout << "</div></div>" << std::endl;

    // End #header.
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_page_footer(void) const
{
    // Begin #footer.

    // Google AdSense
    std::cout << "<br />" << std::endl;
    std::cout << "<br />" << std::endl;
    std::cout << "<div align=\"center\">" << std::endl;
    std::cout << "<script type=\"text/javascript\"><!--" << std::endl;
    std::cout << "google_ad_client = \"pub-5093190295525971\";" << std::endl;
    std::cout << "google_ad_width = 728;" << std::endl;
    std::cout << "google_ad_height = 90;" << std::endl;
    std::cout << "google_ad_format = \"728x90_as\";" << std::endl;
    std::cout << "google_ad_type = \"text_image\";" << std::endl;
    std::cout << "google_ad_channel = \"3511079377\";" << std::endl;
    std::cout << "google_color_border = \"336699\";" << std::endl;
    std::cout << "google_color_bg = \"FFFFFF\";" << std::endl;
    std::cout << "google_color_link = \"0000FF\";" << std::endl;
    std::cout << "google_color_text = \"000000\";" << std::endl;
    std::cout << "google_color_url = \"008000\";" << std::endl;
    std::cout << "//--></script>" << std::endl;
    std::cout << "<script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">" << std::endl;
    std::cout << "</script>" << std::endl;
    std::cout << "</div>" << std::endl;

    std::cout << "<div id=\"footer\"><div><div><hr />" << std::endl;
    std::cout << "<p>Copyright &copy; 2005 Andrei Nicholson</p>" << std::endl;
    std::cout << "</div></div></div>" << std::endl;
    // End #footer.

    std::cout << "</div>" << std::endl;
    // End #content

    std::cout << "</body>" << std::endl;
    std::cout << "</html>";
}

// ----------------------------------------------------------------------------
// @param db - An active connection object.
// @param bAdmin - The admin page uses this same class. If we're being called
//                 from there, prefix the base URL with the admin directory.
// ----------------------------------------------------------------------------
void Layout::print_sidebar(pqxx::connection& db, bool bAdmin)
{
    std::string strBaseUrl(base_url);

    if (bAdmin)
    {
        strBaseUrl += "admin/";
    }

    // Begin #sidebar.
    std::cout << "<div id=\"sidebar\">" << std::endl;

    print_sidebar_profile(strBaseUrl.c_str());

    print_sidebar_calendar(db);

    // Begin .box
    // Begin .box2
    // Begin .box3
    std::cout << "<div class=\"box\"><div class=\"box2\"><div class=\"box3\">" << std::endl;

    print_sidebar_recent_posts(strBaseUrl.c_str(), db);

    print_sidebar_recent_comments(strBaseUrl.c_str(), db);

    print_sidebar_archive(strBaseUrl.c_str(), db);

    std::cout << "</div></div></div>" << std::endl;
    // End .box3
    // End .box2
    // End .box

    print_sidebar_qr();


    // These are going to be outside of any box and borderless.
    // Begin #feeds.
    std::cout << "<div align=\"center\" id=\"feeds\">" << std::endl;
    print_feed_img("atom", "Atom");
    print_feed_img("rss", "RSS");
    print_feed_img("rdf", "RDF");
    std::cout << "</div>" << std::endl;
    // End #feeds.


    std::cout << "</div>" << std::endl;
    // End #sidebar.
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_sidebar_profile(const char* base_url) const
{
    // Begin #profile-container.
    std::cout << "<div id=\"profile-container\">" << std::endl;
    std::cout << "<h2 class=\"sidebar-title\">" << _("Profile") << "</h2>" << std::endl;

    std::cout << "<p>";
    std::cout << "<strong>" << _("Name") << ":</strong> " << _("Andrei Nicholson") << "<br />"
              << "<strong>" << _("Location") << ":</strong> " << blog_attributes.profile_location << "<br />"
              << "<strong>" << _("Email") << ":</strong> <a href=\"mailto:andre@neo-anime.org\">andre@neo-anime.org</a>";
    std::cout << "</p>" << std::endl;
/*
    std::cout << "<dl class=\"profile-datablock\">" << std::endl;
    std::cout << "<dt class=\"profile-img\"><img src=\"img/profile.jpg\" width=\"68\" height=\"80\" alt=\"\" /></dt>" << std::endl;
    std::cout << "<dd class=\"profile-data\"><strong>Name:</strong> Andrei Nicholson</dd>" << std::endl;
    std::cout << "<dd class=\"profile-data\"><strong>Email:</strong> <a href=\"mailto:andre@neo-anime.org\">andre@neo-anime.org</a></dd>" << std::endl;
    std::cout << "</dl>" << std::endl;
*/
    //std::cout << "<p class=\"profile-textblock\">私は愛知県日進市でこの春勉強するになる。</p>" << std::endl;
    std::cout << "<p class=\"profile-textblock\">" << blog_attributes.profile_comment  << "</p>" << std::endl;
    std::cout << "<p class=\"profile-link\"><a href=\"" << base_url << "?eid=4\">" << _("About this blog") << "</a></p>" << std::endl;

    std::cout << "</div>" << std::endl;
    // End #profile-container.
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_sidebar_recent_posts(const char* base_url, pqxx::connection& db)
{
    pqxx::work w(db);
    pqxx::result res;

    std::cout << "<h2 class=\"sidebar-title\">" << _("Recent Posts") << "</h2>" << std::endl;
    std::cout << "<ul id=\"recently\">" << std::endl;

    try
    {
        res = w.exec(""
            "SELECT id, title "
            "FROM blog_entry "
            "ORDER BY posttime DESC "
            "LIMIT 10");
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        std::cout << "</ul>" << std::endl;
        return;
    }

    for (unsigned short i = 0; i != res.size(); ++i)
    {
        std::cout << "<li><a href=\"" << base_url << "?eid=" << res[i]["id"] << "\">" << res[i]["title"] << "</a></li>" << std::endl;
    }

    std::cout << "</ul>" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_feed_img(const char* feedType, const char* feedName) const
{
    std::string getThe = "Get the ";
    getThe += feedName;
    getThe += " feed";

    std::cout << "<p><a href=\"" << base_url << "feed.xml?type=" << feedType << "\">"
              << "<img src=\"" << base_url << "images/button_" << feedType << ".png\" "
              << "alt=\"" << _(getThe.c_str()) << "\" title=\"" << _(getThe.c_str()) << "\" border=\"0\" />"
              << "</a></p>"
              << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_sidebar_calendar(pqxx::connection& db)
{
    unsigned short i;

    pqxx::work w(db);
    pqxx::result res;
    char sql[1000];

    // We'll increment through the res object manually one by one. This will be
    // used as the counter.
    pqxx::result::size_type res_index = 0;

    // All used to find out the current month and year.
    char timeBuffer[20];
    time_t now = time(NULL);
    struct tm* tm_now = localtime(&now);

    an::Cal CCal(static_cast <unsigned short>((tm_now->tm_year + 1900)),
        static_cast <unsigned short>(tm_now->tm_mon + 1));

    // The day of the first week that the first of this month falls on.
    unsigned short start_day = CCal.FirstDayOfMonth();

    // The total number of days in this month.
    unsigned short end_day = CCal.HowManyDays();

    // Counts how many days have been output from the current week. When this
    // number reaches seven, a new row should be created.
    unsigned short weekdays_output = 0;

    // The current day of the month to output.
    unsigned short day_of_month = 1;

    strftime(timeBuffer, sizeof(timeBuffer), flip_date(CALENDAR), tm_now);


    try
    {
        // Select all distinct days for this current month.
        // All posttime values are Epoch. Convert them into a timestamp and then
        // extract just the day portion
        sprintf(sql,
            "SELECT DISTINCT EXTRACT(DAY FROM TIMESTAMP WITH TIME ZONE 'epoch' + posttime * INTERVAL '1 second') AS day "
            "FROM blog_entry "
            "WHERE posttime >= EXTRACT(EPOCH FROM DATE '%d-%d-01') "
            "ORDER BY day ASC",
            static_cast <unsigned short>((tm_now->tm_year + 1900)),
            static_cast <unsigned short>((tm_now->tm_mon + 1)));

        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Do nothing, the res object will remain empty and no dates in the calendar
        // will be marked.
    }


    // Begin .box
    // Begin .box2
    // Begin .box3
    std::cout << "<div class=\"box\"><div class=\"box2\"><div class=\"box3\">" << std::endl;
    //std::cout << "<h2 class=\"sidebar-title\">" << _("Calendar") << "</h2>" << std::endl;
    std::cout << "<h2 class=\"sidebar-title\">" << timeBuffer << "</h2>" << std::endl;

    std::cout << "<center>" << std::endl;
    std::cout << "<table id=\"profile-calendar\" cellspacing=\"0\" cellpadding=\"0\" summary=\"" << _("This month's calendar") << "\">" << std::endl;
    //std::cout << "<caption>" << timeBuffer << "</caption>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Sunday") << "\">" << _("Sun") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Monday") << "\">" << _("Mon") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Tuesday") << "\">" << _("Tue") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Wednesday") << "\">" << _("Wed") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Thursday") << "\">" << _("Thu") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Friday") << "\">" << _("Fri") << "</th>" << std::endl;
    std::cout << "<th scope=\"col\" title=\"" << _("Saturday") << "\">" << _("Sat") << "</th>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;

    // It's possible that the first day doesn't fall on a Sunday. In which case,
    // output blank table cells.
    for (i = 0; i < start_day; ++i)
    {
        std::cout << "<td class=\"npos\">&nbsp;</td>" << std::endl;
        weekdays_output++;
    }

    for (i = start_day; i < (start_day + end_day); ++i)
    {
        // If we've already output seven days, end this table row and begin
        // a new one.
        if (weekdays_output == 7)
        {
            std::cout << "</tr>" << std::endl;
            std::cout << "<tr>" << std::endl;

            weekdays_output = 0;
        }

        // Tag today's date cell.
        if (day_of_month == tm_now->tm_mday)
        {
            std::cout << "<td class=\"today\">";
        }
        else
        {
            std::cout << "<td>";
        }


        try
        {
            // See if this index exists. When we reach the last result set, we'll
            // still increment res_index, putting it out of range -- but we don't
            // know it.
            res.at(res_index);

            if (atoi(res[res_index]["day"].c_str()) == day_of_month)
            {
                sprintf(timeBuffer, "%d-%02d-%02d", static_cast <unsigned short>(tm_now->tm_year + 1900),
                    static_cast <unsigned short>(tm_now->tm_mon + 1), day_of_month);

                std::cout << "<a href=\"?did=" << timeBuffer << "\">" << day_of_month << "</a>";

                res_index++;
            }
            else
            {
                std::cout << day_of_month;
            }
        }
        catch (PGSTD::out_of_range)
        {
            std::cout << day_of_month;
        }

        std::cout << "</td>" << std::endl;

        day_of_month++;
        weekdays_output++;
    }

    // It's possible that the last day of the month doesn't fall on a Saturday.
    // In which case, output blank table cells until we've filled up the
    // remainder of the week.
    for (i = weekdays_output; i < 7; ++i)
    {
        std::cout << "<td class=\"npos\">&nbsp;</td>" << std::endl;
    }

    std::cout << "</tr>" << std::endl;

    std::cout << "</table>" << std::endl;
    std::cout << "</center>" << std::endl;

    std::cout << "</div></div></div>" << std::endl;
    // End .box3
    // End .box2
    // End .box
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_sidebar_recent_comments(const char* base_url, pqxx::connection& db)
{
    pqxx::work w(db);
    pqxx::result res;

    try
    {
        res = w.exec(""
            "SELECT id, blog_id, name, posttime "
            "FROM blog_comment "
            "ORDER BY posttime DESC "
            "LIMIT 10");
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Do nothing.
    }

    std::cout << "<h2 class=\"sidebar-title\">" << _("Recent Comments") << "</h2>" << std::endl;

    std::cout << "<ul class=\"archive-list\">" << std::endl;

    for (pqxx::result::size_type i = 0; i != res.size(); ++i)
    {
        std::cout << "<li><a href=\"" << base_url
                  << "?eid=" << res[i]["blog_id"]
                  << "#" << res[i]["id"]
                  << "\">"
                  << res[i]["name"]
                  << "</a>";
        std::cout << " (" << format_date(res[i]["posttime"].c_str(), ARCHIVE_COMMENT) << ")";
        std::cout << "</li>" << std::endl;
    }

    std::cout << "</ul>" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Layout::print_sidebar_archive(const char* base_url, pqxx::connection& db)
{
    pqxx::work w(db);
    pqxx::result res;

    // Number of entries in the current month we're looking at.
    unsigned short monthCount = 0;

    std::string previousDate;
    std::string currentDate;

    std::string previousTimestamp;
    std::string currentTimestamp;

    try
    {
        // Grab all posttime values from database. This could be a large result
        // set, but the archives needs to display all months from all years available.
        res = w.exec(""
            "SELECT posttime "
            "FROM blog_entry "
            "ORDER BY posttime DESC");
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Do nothing.
    }

    std::cout << "<h2 class=\"sidebar-title\">" << _("Archives") << "</h2>" << std::endl;

    std::cout << "<ul class=\"archive-list\">" << std::endl;

    // Grab the first record's year and month.
    previousDate = format_date(res[0]["posttime"].c_str(), "%Y-%m-01 00:00:00");

    previousTimestamp = res[0]["posttime"].c_str();

    for (pqxx::result::size_type i = 0; i != res.size(); ++i)
    {
        currentDate = format_date(res[i]["posttime"].c_str(), "%Y-%m-01 00:00:00");
        currentTimestamp = res[i]["posttime"].c_str();

        // If the current year and month don't match the previous, output the
        // year and month along with how many entries were counted.
        if (currentDate != previousDate)
        {
            print_sidebar_archive_month(base_url, previousDate.substr(0, 7).c_str(), previousTimestamp.c_str(), monthCount);

            // Reset the month count now that we're looking at a new month, and
            // set the previous month to the new month.
            monthCount = 1;
            previousDate = currentDate;
            previousTimestamp = currentTimestamp;
        }
        // Otherwise we're still looking at the same year and month. Increment
        // the entry count and move on;
        else
        {
            monthCount++;
        }
    }

    // Note: This if() statement should probably be removed at a later date.
    //
    // If there are only one month's worth of entries, the above for() loop will
    // have completed without ever having output anything (because the dates
    // never changed). In this case, monthCount won't be one.
    if (monthCount != 1)
    {
        print_sidebar_archive_month(base_url, currentDate.substr(0, 7).c_str(), currentTimestamp.c_str(), monthCount);
    }

    std::cout << "</ul>" << std::endl;
}

// ----------------------------------------------------------------------------
// Used by print_sidebar_archive(), displays a link for the "year month" along
// with displaying how many entries were in that set.
//
// @param base_url - The base URL used in the anchor link.
// @param mid - Month ID to pass to the index page.
// @param timestamp - The timestamp used to pass to strftime().
// @param monthCount - Number of entries in this year-month set.
// ----------------------------------------------------------------------------
void Layout::print_sidebar_archive_month(const char* base_url, const char* mid,
                                         const char* timestamp, unsigned short monthCount)
{
    std::cout << "<li><a href=\"" << base_url << "?mid=" << mid << "\">";
    std::cout << format_date(timestamp, ARCHIVE_MONTH);
    std::cout << " (" << monthCount << ")</a></li>" << std::endl;
}

// ----------------------------------------------------------------------------
// Displays the QR code in a block.
// ----------------------------------------------------------------------------
void Layout::print_sidebar_qr()
{
    // Begin .box
    // Begin .box2
    // Begin .box3
    std::cout << "<div class=\"box\"><div class=\"box2\"><div class=\"box3\">" << std::endl;
    std::cout << "<h2 class=\"sidebar-title\">" << _("QR Code") << "</h2>" << std::endl;

    std::cout << "<img src=\"images/qr_code.gif\" class=\"c\" alt=\"" << _("QR Code") << "\" title=\"" << _("QR Code") << "\" />"
              << std::endl;

    std::cout << "</div></div></div>" << std::endl;
    // End .box3
    // End .box2
    // End .box
}

// ----------------------------------------------------------------------------
// Begin div#main - Contains main-column blog content.
// ----------------------------------------------------------------------------
void Layout::print_main_column_begin(void) const
{
    // Begin div#mainouter
    std::cout << "<div id=\"mainouter\">" << std::endl;
}

// ----------------------------------------------------------------------------
// End div#main.
// ----------------------------------------------------------------------------
void Layout::print_main_column_end(void) const
{
    std::cout << "</div>" << std::endl;
    // End div#mainouter
}

// ----------------------------------------------------------------------------
// Good-ol' PHP print_input() function converted to C++. This function is too
// slow because of all the string objects!
//
// @param strType - Input type.
// @param strName - Input name.
// @param strValue - Default input value.
// @param lpszSize - Input size (if applicable).
// @param lpszMaxLength - Input maximum allowed input length (if applicable).
// @param strSelected - For checkbox type, whether to select by default.
// @param extra - Any extra attribute to add to the input, such as onclick.
// @return The constructed input tag.
// ----------------------------------------------------------------------------
const char* Layout::print_input(const std::string strType, const std::string strName,
                                const std::string strValue, const char* lpszSize,
                                const char* lpszMaxLength, const std::string strSelected,
                                const char* extra)
{
    std::string ret;

    ret = "<input type=\"" + strType + "\" name=\"" + strName + "\" " + "value=\"" + strValue + "\" ";

    if (strType == "text")
    {
        ret += "size=\"";
        ret += lpszSize;
        ret += "\" maxlength=\"";
        ret += lpszMaxLength;
        ret += "\" ";
    }

    if (strcmp(extra, "") != 0)
    {
        ret += extra;
        ret += " ";
    }

    if (strSelected.size())
    {
        ret += strSelected + "=\"" + strSelected + "\" ";
    }

    ret += "/>";

    return ret.c_str();
}

// ----------------------------------------------------------------------------
// This function would be nice to call from the constructor but we need to
// access the DB for all of our settings.
// ----------------------------------------------------------------------------
void Layout::fetch_settings(pqxx::connection& db)
{
    pqxx::work w(db);
    pqxx::result res;

    try
    {
        res = w.exec(""
            "SELECT * "
            "FROM blog_attributes");
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        return;
    }

    if (!res.size())
    {
        return;
    }

    blog_attributes.blog_name = res[0]["blog_name"].c_str();
    blog_attributes.profile_location = res[0]["profile_location"].c_str();
    blog_attributes.profile_comment = res[0]["profile_comment"].c_str();
}

// ----------------------------------------------------------------------------
// Examines the browser's accepted languages variabled -- retrieved from the
// envionment variables -- and if the preferred language is one of the ones we
// have available, uses it to set the global locale.
//. If the
user accepts Japanese then Fusion will set the locale to Japanese; otherwise
it defaults to English. Other locals could be easily added if desired. Uses
"blog" as the package name to find LC_MESSAGES.
// Using C-style mechanisms because I couldn't find any reference for C++ methods.
// ----------------------------------------------------------------------------
void Layout::init_i18n(void)
{
    // If for whatever reason something goes wrong, default to English.
    const char* lpszDefaultLang = "en_US.utf8";

    // Some visitors to the page might not have this environment variable set,
    // like robots/crawlers or even some weird browsers.
    if (!getenv("HTTP_ACCEPT_LANGUAGE"))
    {
        return;
    }

    std::string accept_lang = getenv("HTTP_ACCEPT_LANGUAGE");

    if (!accept_lang.length())
    {
        return;
    }

    // If there are more than one languages listed, they will be seperated by
    // commas. If no comma is found, then there's only one listed.
    std::string::size_type index = accept_lang.find_first_of(",");

    // Grab the first language -- the preferred one.
    if (index != std::string::npos)
    {
        accept_lang = accept_lang.substr(0, index);
    }

    // Japanese.
    if (accept_lang.find("ja") != std::string::npos)
    {
        m_accept_lang = "ja_JP.utf8";
        m_i18n_jp = true;
    }
    // English.
    else if (accept_lang.find("en") != std::string::npos)
    {
        m_accept_lang = lpszDefaultLang;
    }

    setlocale(LC_ALL, m_accept_lang.c_str());
    bindtextdomain("blog", "locale");
    textdomain("blog");
}

// ----------------------------------------------------------------------------
// Accepts a predetermined date format, format, and a timestamp. This will look
// at the current locale set from init_i18n() to set the appropriate date format.
//
// @param sql_timestamp - Seconds from UNIX Epoch.
// @param format - A value from the date_formats enum.
// ----------------------------------------------------------------------------
const char* Layout::format_date(const std::string sql_timestamp, unsigned short int format)
{
    return create_date(sql_timestamp.c_str(), flip_date(format)).c_str();
}

// ----------------------------------------------------------------------------
// Overloaded version. Accepts an arbitrary strftime() format string and timestamp.
//
// @param sql_timestamp - Seconds from UNIX Epoch.
// @param format - A valid string for strftime().
// ----------------------------------------------------------------------------
const char* Layout::format_date(const std::string sql_timestamp, const char* format)
{
    return create_date(sql_timestamp.c_str(), format).c_str();
}

// ----------------------------------------------------------------------------
// The first incarnation of this function took an SQL TIMESTAMP, broke out each
// of the pieces, and manually set a tm structure's members. This worked okay
// until I discovered that I needed tm_wday filled out and it was damn-near
// impossible to figure out the day of the week from an SQL TIMESTAMP alone.
//
// I scrapped this idea, changed the DB schema to store seconds from Epoch, and
// now feed that in to a time_t variable -- let C do all the work.
//
// @param sql_timestamp - Seconds from UNIX Epoch.
// @param format - A valid string for strftime().
// ----------------------------------------------------------------------------
std::string Layout::create_date(const char* timestamp, const char* format)
{
    // The returned date string.
    std::string pretty_date;

    // This should be large enough to accomodate enough characters.
    char timeBuffer[60];

    time_t then = atoi(timestamp);
    struct tm* tmDate;
    tmDate = localtime(&then);

    strftime(timeBuffer, sizeof(timeBuffer), format, tmDate);

    std::string ret(timeBuffer);
    return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const bool Layout::get_display_user_email(void) const
{
    return m_display_entry_comment_email;
}

} // namespace blog_layout
