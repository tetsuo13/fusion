/**
 * Fusion. Main/Front page to the blog web site.
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
#include <string>
#include <pqxx/pqxx>
#include <ctime>

#include "common.h"
#include "Layout.h"
#include "Cgi.h"
#include "FormInput.h"
#include "Mail.h"

// Defined for the underscore function in common.h
using namespace blog_layout;


// ----------------------------------------------------------------------------
// Queries the DB for the number of comments for a given entry.
//
// @param db - An active connection object.
// @param entry_id - The entry idenfitier.
// @return The number of comments or zero if any errors occur.
// ----------------------------------------------------------------------------
unsigned short num_entry_comments(pqxx::connection& db, const std::string& entry_id)
{
    pqxx::work w(db);
    pqxx::result res;
    std::string sql = "SELECT COUNT(id) AS num_comments "
                      "FROM blog_comment "
                      "WHERE blog_id = " + entry_id;

    try
    {
        res = w.exec(sql.c_str());
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Don't bother doing anything for an exception; just return zero.
        return 0;
    }

    return static_cast <unsigned short>(atoi(res[0]["num_comments"].c_str()));
}

// ----------------------------------------------------------------------------
// This function should take every letter of the supplied email address and
// swap it out for its HTML representation.
// ----------------------------------------------------------------------------
const char* obfuscate_email(std::string email, blog_cgi::Cgi& CCgi)
{
    /*
    // Table of alphabet (A-Z) converted to HTML entity.
    static const char cEntityAlphabet[26][7] =
    {
        "&#097;", "&#098;", "&#099;", "&#100;", "&#101;", "&#102;", "&#103;",
        "&#104;", "&#105;", "&#106;", "&#107;", "&#108;", "&#109;", "&#110;",
        "&#111;", "&#112;", "&#113;", "&#114;", "&#115;", "&#116;", "&#117;",
        "&#118;", "&#119;", "&#120;", "&#121;", "&#122;"
    };

    // Table of number (0-9) converted to HTML entity.
    static const char cEntityNumeric[10][7] =
    {
        "&#048;", "&#049;", "&#050;", "&#051;", "&#052;", "&#053;", "&#054;",
        "&#055;", "&#056;", "&#057;"
    };
    */

    CCgi.replace_character(email, ".", "&#046;");
    CCgi.replace_character(email, "@", "&#064;");

    return email.c_str();
}

// ----------------------------------------------------------------------------
// Print JavaScript inline.
// ----------------------------------------------------------------------------
void print_javascript(blog_layout::Layout& CLayout)
{
    std::cout << "<script language=\"javascript\">" << std::endl
              << "//<![CDATA[" << std::endl;

    // Should be called as the onsubmit action to the form. Will disable the
    // selected form input button, disallowing user to submit twice.
    std::cout << "function submitOnce(button)"          << std::endl
              << "{"                                    << std::endl
              << "    button.disabled = true;"          << std::endl
              << "    return true;"                     << std::endl
              << "}"                                    << std::endl;

    std::cout << "//]]>" << std::endl
              << "</script>" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_block_start(void)
{
    // Begin #main
    // Begin #main2
    // Begin #main3
    std::cout << "<div id=\"main\"><div id=\"main2\"><div id=\"main3\">" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_block_end(void)
{
    std::cout << "</div></div></div>" << std::endl;
    // End #main3
    // End #main2
    // End #main
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_comment_table(pqxx::connection& db, blog_layout::Layout& CLayout,
                         blog_cgi::Cgi& CCgi, form_input::FormInput& CFormInput,
                         const std::string& entry_id, const bool allow_comments)
{
    bool bAltComment = false;
    pqxx::work w(db);
    pqxx::result res;
    std::string sql = "SELECT id, posttime, name, email, homepage, body "
                      "FROM blog_comment "
                      "WHERE blog_id = " + entry_id + " "
                      "ORDER BY posttime ASC";

    try
    {
        res = w.exec(sql.c_str());
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        return;
    }

    print_block_start();

    std::cout << "<div id=\"comments\">" << std::endl;
    std::cout << "<a name=\"comments\"></a>" << std::endl;
    std::cout << "<p class=\"comment-body\"><h3>" << _("Comments") << "</h3></p>" << std::endl;
    std::cout << "</div>" << std::endl;

    for (pqxx::result::size_type i = 0; i != res.size(); ++i)
    {
        if (bAltComment)
        {
            std::cout << "<div id=\"comments-alt\">" << std::endl;
        }
        else
        {
            std::cout << "<div id=\"comments\">" << std::endl;
        }

        std::cout << "<a name=\"" << res[i]["id"].c_str() << "\"></a>" << std::endl;

        std::cout << "<p class=\"comment-data\">" << res[i]["body"].c_str() << "</p>" << std::endl;

        std::cout << "<p class=\"comment-body\">" << _("Name") << ": ";

        // If an email address is present, enclose the poster's name in an mailto
        // anchor tag. Otherwise simply display their name as-is.
        if (CLayout.get_display_user_email() == true &&
            static_cast <int>(strlen(res[i]["email"].c_str())))
        {
            std::cout << "<a href=\"&#109;&#097;&#105;&#108;&#116;&#111;:" << obfuscate_email(res[i]["email"].c_str(), CCgi)
                      << "\" class=\"comment-poster\" rel=\"nofollow\">"
                      << res[i]["name"].c_str() << "</a> ";
        }
        else
        {
            std::cout << res[i]["name"].c_str() << " ";
        }

        if (static_cast <int>(strlen(res[i]["homepage"].c_str())))
        {
            std::cout << "| <a href=\"" << res[i]["homepage"].c_str() << "\" target=\"_blank\" title=\"" << res[i]["homepage"].c_str() << "\">"
                      << _("Homepage") << "</a> ";
        }

        std::cout << "| " << CLayout.format_date(res[i]["posttime"].c_str(), blog_layout::COMMENT) << " EST";
        std::cout << "</p>" << std::endl;

        // End div#comments.
        std::cout << "</div>" << std::endl;

        bAltComment = !bAltComment;
    }

    print_block_end();

    // If posting comments aren't allowed because this entry is out of date,
    // don't display the form to submit comments.
    if (!allow_comments)
    {
        return;
    }

    print_javascript(CLayout);

    print_block_start();

    std::cout << "<form method=\"post\" action=\"\" onsubmit=\"submitOnce(this.c_submit)\">" << std::endl;
    std::cout << CLayout.print_input("hidden", "entry_id", entry_id.c_str())
              << std::endl;

    std::cout << "<div id=\"comments\">" << std::endl;

    std::cout << "<p class=\"comment-body\"><h3>" << _("Write A Comment") << "</h3></p>" << std::endl;

    std::cout << CFormInput.print_error("general");

    std::cout << "<p class=\"comment-body\">" << _("Name") << " <span class=\"required\">*</span><br />"
              << CFormInput.print_error("c_name")
              << CLayout.print_input("text", "c_name", CCgi.get_post("c_name", true), "20", "25")
              << "</p>" << std::endl;

    std::cout << "<p class=\"comment-body\">" << _("Email") << "<br />"
              << CFormInput.print_error("c_email")
              << CLayout.print_input("text", "c_email", CCgi.get_post("c_email", true), "30", "120")
              << "<br />";

    if (CLayout.get_display_user_email() == true)
    {
        std::cout << _("Note: If entered, email address will be obfuscated.");
    }
    else
    {
        std::cout << _("Note: If entered, email address will not be displayed.");
    }

    std::cout << "</p>" << std::endl;

    std::cout << "<p class=\"comment-body\">" << _("Homepage") << "<br />"
              << CFormInput.print_error("c_homepage")
              << CLayout.print_input("text", "c_homepage", CCgi.get_post("c_homepage", true), "30", "256")
              << "</p>" << std::endl;

    std::cout << "<p class=\"comment-body\">" << _("Comment") << " <span class=\"required\">*</span><br />"
              << CFormInput.print_error("c_body")
              << "<textarea name=\"c_body\" rows=\"13\" cols=\"47\">"
              << CCgi.get_post("c_body", true) << "</textarea>"
              << "<br />" << _("Note: HTML tags are not allowed.") << "<br />" << std::endl;

    std::cout << "<div align=\"center\">"
              << CLayout.print_input("submit", "c_submit", _("Submit Comment"))
              << "</div>" << std::endl;

    // End div#comments.
    std::cout << "</div>" << std::endl;

    std::cout << "</form>" << std::endl;

    print_block_end();
}

// ----------------------------------------------------------------------------
// A 'year-month' combination will come in the form of 'YYYYY-MM'. Issue a simple
// LIKE query to see if anything like it exists.
// ----------------------------------------------------------------------------
void verify_month_id(pqxx::connection& db, std::string& month_id)
{
    pqxx::work w(db);
    pqxx::result res;

    std::string sql = "SELECT posttime "
                      "FROM blog_entry "
                      "WHERE posttime >= EXTRACT(EPOCH FROM TIMESTAMP '" + month_id + "-01') "
                      "LIMIT 1";

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // If anything went wrong in the query, reset the month id.
        month_id.clear();
        return;
    }

    // A valid 'year-month' combination was specified, however there aren't any
    // such posttimes that are like it. Clear out the month id.
    if (res.empty())
    {
        month_id.clear();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void verify_day_id(pqxx::connection& db, std::string& day_id)
{
    pqxx::work w(db);
    pqxx::result res;

    std::string sql = "SELECT posttime "
                      "FROM blog_entry "
                      "WHERE posttime >= EXTRACT(EPOCH FROM TIMESTAMP '" + day_id + "') AND "
                      "      posttime <= EXTRACT(EPOCH FROM date '" + day_id + "' + INTERVAL '1 day') "
                      "LIMIT 1";

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception e)
    {
        // Something went wrong in the query (probably user malformed data).
        day_id.clear();
        return;
    }

    // If no results were found, it means that the year-month-day combination
    // doesn't contain any posts -- probably due to user entering arbitrary dates.
    if (res.empty())
    {
        day_id.clear();
    }
}

// ----------------------------------------------------------------------------
// User-given data is untrustable! Look in the database for an existing entry
// id. The user can give character, interger, mixed, etc, try/catch statement
// will take care of it.
// ----------------------------------------------------------------------------
void verify_entry_id(pqxx::connection& db, std::string& entry_id)
{
    pqxx::work w(db);
    pqxx::result res;

    std::string sql = "SELECT id "
                      "FROM blog_entry "
                      "WHERE id = " + entry_id;

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Should anything go wrong at this point, reset the entry id;
        // something is wrong with it!
        entry_id.clear();
        return;
    }

    // A valid numeric value was given but it doesn't exist in the blog_entry
    // table. Clear out the entry id.
    if (res.empty())
    {
        entry_id.clear();
    }
}

// ----------------------------------------------------------------------------
// This could have been implemented with C calls, but I couldn't figure it out.
// Queries the DB engine to find out a date in the past in seconds since Epoch.
//
// @return One month ago from now, in seconds from Epoch.
// ----------------------------------------------------------------------------
std::string get_cutoff_date(pqxx::connection& db)
{
    pqxx::work w(db);
    pqxx::result res;

    // Grab the current time in seconds from Epoch. Then subtract from it one
    // month's worth of seconds.
    const char* sql = "SELECT EXTRACT(EPOCH FROM NOW())::bigint - EXTRACT(EPOCH FROM INTERVAL '1 month') AS cutoff";

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // This exception /shouldn't/ ever be raised.
        return "0";
    }

    return res[0]["cutoff"].c_str();
}

// ----------------------------------------------------------------------------
// Prints the main content.
//
// ----------------------------------------------------------------------------
void print_main(pqxx::connection& db, blog_layout::Layout& CLayout,
                blog_cgi::Cgi& CCgi, form_input::FormInput& CFormInput,
                std::string& entry_id, std::string& month_id, std::string& day_id)
{
    std::string sql;
    std::string currentDate;

    // Number of comments on the current entry we're looking at.
    unsigned short ushEntryComments = 0;

    // If the current entry we're looking at allows comments.
    bool bAllowComments = true;

    // The oldest allowed timestamp for comments. Anything older than this won't
    // be opened for commenting.
    std::string cutoffDate = get_cutoff_date(db);

    CLayout.print_main_column_begin();

    // If an entry_id was specified, verify that it exists in the database. If
    // anything goes wrong, the entry id will be reset and we'll display the
    // page as if no entry id was given in the first place.
    if (entry_id.size())
    {
        verify_entry_id(db, entry_id);
    }
    // Otherwise if a 'year-month' combo was specified, verify that it's valid.
    else if (month_id.size())
    {
        verify_month_id(db, month_id);
    }
    else if (day_id.size())
    {
        verify_day_id(db, day_id);
    }

    sql = "SELECT id, posttime, body, title, allow_comments "
          "FROM blog_entry ";

    if (entry_id.size())
    {
        sql += "WHERE id = " + entry_id;
    }
    else if (month_id.size())
    {
        // This is a little bit ugly but I couldn't figure a better way of doing
        // calendar date math using Epoch seconds.
        //
        // Limit to only entries whose posttime is greater than Epoch seconds
        // from the start of the month to Epoch seconds at the end of the month;
        // find out how many seconds are in that month.
        sql += "WHERE posttime >= EXTRACT(EPOCH FROM TIMESTAMP '" + month_id + "-01') AND "
               "      posttime < "
               "      ("
               "          EXTRACT(EPOCH FROM TIMESTAMP '" + month_id + "-01') + "
               "          ("
               "              EXTRACT(EPOCH FROM date '" + month_id + "-01' + INTERVAL '1 month') - "
               "              EXTRACT(EPOCH FROM TIMESTAMP '" + month_id + "-01')"
               "          )"
               "      ) "
               "ORDER BY posttime DESC";
    }
    else if (day_id.size())
    {
        // This query is very similar to the one directly above, huh.
        sql += "WHERE posttime >= EXTRACT(EPOCH FROM TIMESTAMP '" + day_id + "') AND "
               "      posttime < "
               "      ("
               "          EXTRACT(EPOCH FROM TIMESTAMP '" + day_id + "') + "
               "          ("
               "              EXTRACT(EPOCH FROM date '" + day_id + "' + INTERVAL '1 day') - "
               "              EXTRACT(EPOCH FROM TIMESTAMP '" + day_id + "')"
               "          )"
               "      ) "
               "ORDER BY posttime DESC";
    }
    else
    {
        sql += "ORDER BY posttime DESC "
               "LIMIT 15";
    }

    pqxx::work w(db);
    pqxx::result res;

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // Something went wrong (possible user input error) and the SQL query
        // barfed.
        //std::cout << "<p><strong>Error</strong> " << e.what() << sql << "</p>" << std::endl;
        CLayout.print_main_column_end();
        return;
    }

    for (pqxx::result::size_type i = 0; i != res.size(); ++i)
    {
        // If the entry has been flagged as not allowing comments, don't allow
        // users to. Also, entries older than a certain amount should be
        // considered "closed" for commenting.
        if (strcmp(res[i]["allow_comments"].c_str(), "f") == 0 ||
            atoi(res[i]["posttime"].c_str()) < atoi(cutoffDate.c_str()))
        {
            bAllowComments = false;
        }
        else
        {
            bAllowComments = true;
        }

        if (entry_id.size())
        {
            ushEntryComments = num_entry_comments(db, entry_id);
        }
        else
        {
            ushEntryComments = num_entry_comments(db, res[i]["id"].c_str());
        }

        // If a new day comes about, print a new date header.
        if (currentDate != CLayout.format_date(res[i]["posttime"].c_str(), blog_layout::DATE_HEADER))
        {
            if (i != 0)
            {
                print_block_end();
            }

            print_block_start();

            std::cout << "<h2 class=\"date-header\">" << CLayout.format_date(res[i]["posttime"].c_str(), blog_layout::DATE_HEADER)
                      << "</h2>" << std::endl;
        }

        std::cout << "<div class=\"post\"><a name=\"" << res[i]["id"] << "\"></a>" << std::endl;

        // In order to gain complete XHTML validation, the anchor tag must be
        // enclosed within the h3 tag -- not the other way around. But this
        // breaks the CSS layout slightly...
//        std::cout << "<h3 class=\"post-title\"><a href=\"?eid=" << res[i]["id"] << "\" class=\"title-link\">"
//                  << res[i]["title"] << "</a></h3>" << std::endl;

        std::cout << "<a href=\"?eid=" << res[i]["id"] << "\" class=\"title-link\">"
                  << "<h3 class=\"post-title\">" << res[i]["title"] << "</h3></a>" << std::endl;

        std::cout << "<div class=\"post-body\">" << std::endl
                  << res[i]["body"] << std::endl
                  << "</div>" << std::endl;

        std::cout << "<p class=\"post-footer\">" << std::endl
                  << "<em>Posted by Andre @ " << CLayout.format_date(res[i]["posttime"].c_str(), blog_layout::ENTRY_TIME) << " EST</em>" << std::endl;

        if (bAllowComments)
        {
            std::cout << "<a href=\"?eid=" << res[i]["id"] << "#comments\" class=\"comment-link\">" << _("comments")
                      << " (" << ushEntryComments << ")</a>" << std::endl;
        }
        else
        {
            std::cout << "<a class=\"comment-link\"></a>" << std::endl;
        }

        std::cout << "</p>" << std::endl;

        std::cout << "</div>" << std::endl;
        // End .post

        currentDate = CLayout.format_date(res[i]["posttime"].c_str(), blog_layout::DATE_HEADER);
    }

    print_block_end();

    // If we're looking at a single entry and it allows commenting, display its
    // comments.
    if (entry_id.size() && strcmp(res[0]["allow_comments"].c_str(), "f") != 0)
    {
        // If the post time is less than the cutoff time, allow commenting.
        bool out_of_date = (atoi(res[0]["posttime"].c_str()) < atoi(cutoffDate.c_str())) ? false : true;

        print_comment_table(db, CLayout, CCgi, CFormInput, entry_id, out_of_date);
    }

    CLayout.print_main_column_end();
}

// ----------------------------------------------------------------------------
// Do not allow anonymouse visitors to post too often on a single entry.
//
// @param db - An active connection object.
// @param entryId - The blog_entry id to look at.
// @return bool - false if the IP address has posted a comment on this entry in
//                the last hour; true if they haven't.
// ----------------------------------------------------------------------------
bool okay_to_post(pqxx::connection& db, const char* entryId)
{
    pqxx::work w(db);
    pqxx::result res;
    char sql[1000];

    // Damn!
    // Uses PostgreSQL date operators to see if this user's last comment is
    // older than one hour (3600 seconds).
    sprintf(sql,
        "SELECT id "
        "FROM blog_comment "
        "WHERE blog_id = %s AND "
        "      ip = '%s' AND "
        "      EXTRACT(EPOCH FROM NOW())::bigint - 3600 < ("
        "   SELECT posttime "
        "   FROM blog_comment "
        "   WHERE blog_id = %s AND "
        "         ip = '%s' "
        "   ORDER BY posttime DESC "
        "   LIMIT 1) "
        "LIMIT 1",
        entryId,
        getenv("REMOTE_ADDR"),
        entryId,
        getenv("REMOTE_ADDR"));

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        return false;
    }

    if (!res.empty())
    {
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// Checks the syntax of an email address.
//
// @arg email - Email address.
// @return true if the email address is of correct syntax; false otherwise.
// ----------------------------------------------------------------------------
bool valid_email(const std::string& email)
{
    if (email.find("@") == std::string::npos)
    {
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void handle_form_input(pqxx::connection& db, blog_cgi::Cgi& CCgi,
                       blog_layout::Layout& CLayout, form_input::FormInput& CFormInput)
{
    // Total amount of characters in entire POST string, if it's greater than
    // all form inputs' maxlength and some, something probably isn't right.
    if (atoi(getenv("CONTENT_LENGTH")) > 2000)
    {
        CFormInput.set_error("general", _("Form input too large, possible error occurred."));
        return;
    }

    std::string name     = CCgi.get_post("c_name");
    std::string email    = CCgi.get_post("c_email");
    std::string homepage = CCgi.get_post("c_homepage");
    std::string comment  = CCgi.get_post("c_body");
    std::string entry_id = CCgi.get_post("entry_id");

    // Make sure this user hasn't already commented on this entry within the last
    // few hours. Set an error and return otherwise.
    if (!okay_to_post(db, entry_id.c_str()))
    {
        CFormInput.set_error("general", _("You have already commented on this entry not too long ago. Please wait some more -- don&#039;t spam."));
        return;
    }

    if (!static_cast <int>(strlen(name.c_str())))
    {
        CFormInput.set_error("c_name", _("Please enter your name."));
    }

    if (!static_cast <int>(strlen(comment.c_str())))
    {
        CFormInput.set_error("c_body", _("Please leave a comment."));
    }

    // Need to do extensive email syntax checking here only if an email address
    // was supplied (it's optional).
    if (static_cast <int>(strlen(email.c_str())) && !valid_email(email))
    {
        CFormInput.set_error("c_email", _("Invalid email address."));
    }

    // The entry id *should* be available!
    if (!entry_id.length())
    {
        CFormInput.set_error("general", _("Missing entry id. Please retry."));
    }

    if (CFormInput.is_form_okay())
    {
        // Take care of any special characters we need to watch out for.
        CCgi.sanitize_text(name);
        CCgi.sanitize_text(email);
        CCgi.sanitize_text(homepage);
        CCgi.sanitize_text(comment);

        // Convert Windows-style charage returns to UNIX.
        CCgi.replace_character(comment, "\r\n", "\n");

        // Replace charage returns with a break tag for a little nicer formatting.
        CCgi.replace_character(comment, "\n", "<br />");

        // This could lead to a buffer overflow!! Fix!
        char sql[5000];


/*
extern int snprintf (char *__restrict __s, size_t __maxlen,
		     __const char *__restrict __format, ...)
     __THROW __attribute__ ((__format__ (__printf__, 3, 4)));
*/


        sprintf(sql,
            "INSERT INTO blog_comment "
            "(posttime, blog_id, ip, name, "
            "email, homepage, body) "
            "VALUES "
            "(EXTRACT(EPOCH FROM NOW())::bigint, %s, '%s', '%s', "
            "'%s', '%s', '%s')",
            CCgi.get_post("entry_id"), getenv("REMOTE_ADDR"), name.c_str(),
            email.c_str(), homepage.c_str(), comment.c_str());

        try
        {
            pqxx::work insert(db, "NewCommentTransaction");
            insert.exec(sql);
            insert.commit();
        }
        catch (const PGSTD::exception& e)
        {
            CFormInput.set_error("general", e.what());
            return;
        }



        an::Mail CMail;

        CMail.SetFrom("webmaster@neo-anime.org");
        CMail.SetTo("webmaster@neo-anime.org", "Andrei Nicholson");
        CMail.SetSubject("[fusion] New Comment Posted");
        CMail.SetBody(sql);
        CMail.Send();


        std::string location("Location: ");
        location += base_url;
        CLayout.header(location.c_str(), true);

        exit(0);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int main(void)
{
    // GET variable, specifies a single entry id to display.
    std::string entry_id;

    // GET variable, specifies a single 'year-month' to display entries from.
    std::string month_id;

    // GET variable, specifies a single day of entries to display.
    std::string day_id;

    blog_cgi::Cgi CCgi;
    form_input::FormInput CFormInput("<span id=\"error\">", "</span><br />");

    blog_layout::Layout CLayout;
    CLayout.init_i18n();

    try
    {
        pqxx::connection db(db_dsn);

        CLayout.fetch_settings(db);

        // We'll only look at one type of query: POST or GET; never both.
        //
        // Check to see if the form has been submitted (POST).
        if (getenv("CONTENT_LENGTH") != NULL)
        {
            CCgi.parse_post();

            handle_form_input(db, CCgi, CLayout, CFormInput);

            // If code execution made it this far -- ie, we've process the form
            // inputs and returned back to main() afterwards -- something went
            // wrong with the user's input values. Grab the entry id from POST.
            entry_id = CCgi.get_post("entry_id");
        }
        // If GET variables are present, look for an entry id.
        else if (getenv("QUERY_STRING"))
        {
            CCgi.parse_get();

            entry_id = CCgi.get_get("eid");
            month_id = CCgi.get_get("mid");
            day_id = CCgi.get_get("did");
        }

        CLayout.print_page_header("");

        CCgi.parse_post();

        print_main(db, CLayout, CCgi, CFormInput, entry_id, month_id, day_id);

        CLayout.print_sidebar(db);
    }
    catch (const PGSTD::exception& e)
    {
        std::cout << "<p><strong>Error</strong> " << e.what() << "</p>" << std::endl;
    }

    CLayout.print_page_footer();

    return 0;
}
