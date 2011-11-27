/**
 * Fusion. Front-end to administer blog.
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

#include "../Layout.h"
#include "../Cgi.h"
#include "../FormInput.h"
#include "../common.h"


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_entry_comments(pqxx::connection& db, blog_layout::Layout& CLayout,
                          const std::string& strBlogId)
{
    pqxx::work w(db);
    pqxx::result res;

    std::string strSql = "SELECT id, posttime, ip, name, email, homepage, body "
                         "FROM blog_comment "
                         "WHERE blog_id = " + strBlogId;

    try
    {
        res = w.exec(strSql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        std::cout << "<p><strong>Error:</strong> " << e.what() << "</p>" << std::endl;
        return;
    }

    if (res.size() == 0)
    {
        return;
    }

    bool bAltRow = false;
    std::string strCheckboxName;

    std::cout << "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"2\">" << std::endl;

    for (pqxx::result::size_type i = 0; i != res.size(); ++i)
    {
        strCheckboxName  = "d_c__";
        strCheckboxName += res[i]["id"].c_str();

        if (bAltRow)
        {
            std::cout << "<tr id=\"comments-alt\">" << std::endl;
        }
        else
        {
            std::cout << "<tr>" << std::endl;
        }

        std::cout << "<td valign=\"top\" rowspan=\"2\">"
                  << CLayout.print_input("checkbox", strCheckboxName.c_str(), res[i]["id"].c_str())
                  << "</td>" << std::endl;

        std::cout << "<td>"
                  << res[i]["body"].c_str()
                  << "</td>" << std::endl;

        std::cout << "</tr>" << std::endl;

        if (bAltRow)
        {
            std::cout << "<tr id=\"comments-alt\">" << std::endl;
        }
        else
        {
            std::cout << "<tr>" << std::endl;
        }

        std::cout << "<td>";

        if (static_cast <int>(strlen(res[i]["email"].c_str())))
        {
            std::cout << "<a href=\"mailto:" << res[i]["email"].c_str() << "\" title=\"" << res[i]["ip"].c_str() << "\">"
                      << res[i]["name"] << "</a>";
        }
        else
        {
            std::cout << "<span title=\"" << res[i]["ip"].c_str() << "\">" << res[i]["name"].c_str() << "</span>";
        }

        std::cout << " @ " << CLayout.format_date(res[i]["posttime"].c_str(), "%B %e, %Y %l:%M %p");

        if (static_cast <int>(strlen(res[i]["homepage"].c_str())))
        {
            std::cout << " (<a href=\"" << res[i]["homepage"].c_str() << "\" target=\"_blank\">Homepage</a>)";
        }

        std::cout << "</td>" << std::endl;
        std::cout << "</tr>" << std::endl;

        bAltRow = !bAltRow;
    }

    std::cout << "</table>" << std::endl;

    std::cout << "<br />\n<div align=\"center\">"
              << CLayout.print_input("submit", "sub", "Delete Selected Comments")
              << "</div>" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_entry_button(const char* tag, const char* attribute, const char* info,
                        const char* button)
{
    std::cout << "<a href=\"javascript: void(0);\" onclick=\"javascript: addTag('" << tag << "', '" << attribute << "')\" title=\"" << info << "\">" << button << "</a>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_entry_buttons(void)
{
    const char* spacer = " ";

    std::string tmp();
    std::string c();
    std::string base_image_src("src=\\'");
    base_image_src += base_url;
    base_image_src += "/entry_images/\\' alt=\\'\\' title=\\'\\'";

    print_entry_button("p", "", "English paragraph", "P");
    std::cout << spacer;
    print_entry_button("p", "class=\\'flush\\'", "English paragraph (non-indented)", "P.flush");
    std::cout << spacer;
    print_entry_button("p", "class=\\'ja\\'", "Japanese paragraph", "P.ja");
    std::cout << spacer;
    print_entry_button("p", "class=\\'ja-flush\\'", "Japanese paragraph (non-indented)", "P.ja-flush");
    std::cout << spacer;
    print_entry_button("ol", "class=\\'count\\'", "Numbered ordered list", "OL");
    std::cout << spacer;
    print_entry_button("ol", "class=\\'ja\\'", "Japanese ordered list", "OL.ja");
    std::cout << spacer;
    print_entry_button("blockquote", "", "Quote", "QUOTE");
    std::cout << spacer;
    print_entry_button("img", base_image_src.c_str(), "Image", "IMG");
    std::cout << spacer;

    c = " class=\\'c\\'";
    tmp = base_image_src + c;
    print_entry_button("img", tmp.c_str(), "Image (centered)", "IMG.c");
    std::cout << spacer;

    c = " class=\\'right\\'";
    tmp = base_image_src + c;
    print_entry_button("img", tmp.c_str(), "Image (right)", "IMG.right");
    std::cout << spacer;

    c = " class\\'left\\'";
    tmp = base_image_src + c;
    print_entry_button("img", tmp.c_str(), "Image (left)", "IMG.left");
    std::cout << spacer;
}

// ----------------------------------------------------------------------------
// Prints the main content.
// ----------------------------------------------------------------------------
void print_main(pqxx::connection& db, blog_cgi::Cgi& CCgi, blog_layout::Layout& CLayout,
                form_input::FormInput& CFormInput, std::string& strBlogId)
{
    // If we're editing an existing entry.
    bool bEdit = false;

    std::string strSubmitName = "Post Entry";
    std::string strAcChecked = "checked";

    if (strBlogId.size())
    {
        bEdit = true;

        std::string sql = "SELECT posttime, body, title, allow_comments "
                          "FROM blog_entry "
                          "WHERE id = " + strBlogId;

        pqxx::work w(db);
        pqxx::result res;

        try
        {
            res = w.exec(sql);
            w.commit();
        }
        catch (const PGSTD::exception& e)
        {
            std::cout << "<p><strong>Error</strong> " << e.what() << sql << "</p>" << std::endl;
            return;
        }

        CCgi.set_post("subject", res[0]["title"].c_str());
        CCgi.set_post("entry", res[0]["body"].c_str());

        if (strcmp(res[0]["allow_comments"].c_str(), "f") == 0)
        {
            strAcChecked = "";
        }

        strSubmitName = "Save Changes";
    }

    std::cout << "<script type=\"text/javascript\" src=\"functions.js\" />" << std::endl;

    // Begin #main - Contains main-column blog content.
    std::cout << "<div id=\"main\"><div id=\"main2\"><div id=\"main3\">" << std::endl;
    std::cout << "<div class=\"post\"><h3 class=\"post-title\">Create New Entry</h2>" << std::endl;
    std::cout << "<div class=\"post-body\">" << std::endl;

    // Print any error messages that occured when submitting the form. The 'general'
    // key will be set when we tried to execute an SQL query and it failed for
    // some reason, the value will be that reason.
    std::cout << CFormInput.print_error("general");

    std::cout << "<form method=\"post\" action=\"" << getenv("SCRIPT_NAME") << "\">" << std::endl;

    if (bEdit)
    {
        std::cout << CLayout.print_input("hidden", "entry_id", strBlogId.c_str()) << std::endl;
    }

    std::cout << "<table width=\"100%\">" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<th>Title</th>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<td>"
              << CFormInput.print_error("subject")
              << CLayout.print_input("text", "subject", CCgi.get_post("subject"), "40", "256")
              << "</td>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<th>Entry Text</th>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<td>";
    print_entry_buttons();
    std::cout << "<br /><textarea name=\"entry\" rows=\"20\" cols=\"45\">" << CCgi.get_post("entry") << "</textarea></td>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<td colspan=\"2\"><fieldset><legend>Options</legend>";
    std::cout << "<label>"
              << CLayout.print_input("checkbox", "allow_comments", "1", "0", "0", strAcChecked.c_str())
              << " Allow comments</label>";
    std::cout << "</fieldset></td>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<td colspan=\"2\">&nbsp;</td>" << std::endl;
    std::cout << "</tr>" << std::endl;

    std::cout << "<tr>" << std::endl;
    std::cout << "<td colspan=\"2\" align=\"center\">"
              << "<div align=\"center\">"
              << CLayout.print_input("submit", "sub", strSubmitName.c_str());
    std::cout << "&nbsp;&nbsp;"
              << CLayout.print_input("button", "pre", "Preview", "", "", "", "onclick=\"javascript: preview(document.forms[0].entry)\"");
    std::cout << "&nbsp;&nbsp;"
              << CLayout.print_input("submit", "sub", "Cancel")
              << "</td>" << std::endl;
    std::cout << "</tr>" << std::endl;

    if (bEdit)
    {
        std::cout << "<tr>" << std::endl;
        std::cout << "<td colspan=\"2\">&nbsp;</td>" << std::endl;
        std::cout << "</tr>" << std::endl;

        std::cout << "<tr>" << std::endl;
        std::cout << "<td colspan=\"2\"><fieldset><legend>Contributed Comments</legend>" << std::endl;
        print_entry_comments(db, CLayout, strBlogId);
        std::cout << "</fieldset></td>" << std::endl;
        std::cout << "</tr>" << std::endl;

        std::cout << "<tr>" << std::endl;
        std::cout << "<td colspan=\"2\">&nbsp;</td>" << std::endl;
        std::cout << "</tr>" << std::endl;

        std::cout << "<tr>" << std::endl;
        std::cout << "<td colspan=\"2\"><fieldset><legend>Delete Entry</legend>";
        std::cout << "<label>"
                  << CLayout.print_input("checkbox", "delete_entry", "1")
                  << " Delete this entry</label>";
        std::cout << "<br /><div align=\"center\">"
                  << CLayout.print_input("submit", "sub", "Delete")
                  << "</div>";
        std::cout << "</fieldset></td>" << std::endl;
        std::cout << "</tr>" << std::endl;
    }

    std::cout << "</table>" << std::endl;

    std::cout << "</form>" << std::endl;
    std::cout << "</div>" << std::endl;
    std::cout << "</div>" << std::endl;

    std::cout << "</div></div></div>" << std::endl;
    // End #main.
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void delete_entry(pqxx::connection& db, const std::string& strBlogId)
{
    std::string strSql;

    try
    {
        pqxx::work w(db, "DeleteEntryTransaction");

        strSql = "DELETE FROM blog_entry WHERE id = " + strBlogId;
        w.exec(strSql.c_str());

        strSql = "DELETE FROM blog_comment WHERE blog_id = " + strBlogId;
        w.exec(strSql.c_str());

        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        return;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void delete_comments(pqxx::connection& db, blog_cgi::Cgi& CCgi, const std::string& strBlogId)
{
    std::cout << "<pre>";
    CCgi.print_post_vars();
    std::cout << "</pre>" << std::endl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void handle_form_input(pqxx::connection& db, blog_cgi::Cgi& CCgi, blog_layout::Layout& CLayout,
                       form_input::FormInput& CFormInput)
{
    std::string admin_location("Location: ");
    admin_location += base_url;
    admin_location += "/admin/";

    CCgi.parse_post();

    std::string strSubmit = CCgi.get_post("sub");
    std::string strBlogId = CCgi.get_post("entry_id");

    // Canceling everything. Just redirect back to admin homepage.
    if (strSubmit == "Cancel")
    {
        CLayout.header(admin_location.c_str(), true);
        exit(0);
    }
    // Deleting an entry.
    else if (strSubmit == "Delete")
    {
        if (strcmp(CCgi.get_post("delete_entry"), "1") == 0)
        {
            delete_entry(db, strBlogId);
            CLayout.header(admin_location.c_str(), true);
            exit(0);
        }

        return;
    }
    else if (strSubmit == "Delete Selected Comments")
    {
        delete_comments(db, CCgi, strBlogId);
//        CLayout.header(admin_location.c_str(), true);
        exit(0);
    }

    std::string strSubject        = CCgi.get_post("subject");
    std::string strEntry          = CCgi.get_post("entry");
    std::string strAllowComments  = CCgi.get_post("allow_comments");


    if (!strSubject.size())
    {
        CFormInput.set_error("subject", "Please supply a title.");
    }

    if (!strEntry.size())
    {
        CFormInput.set_error("entry", "Please supply an entry body.");
    }

    if (CFormInput.is_form_okay())
    {
        // This could lead to a buffer overflow!! Fix!
        char sql[5000];

        // Take care of any special characters we need to watch out for.
        CCgi.replace_character(strEntry, "&", "&amp;");
        CCgi.replace_character(strSubject, "'", "&#039;");
        CCgi.replace_character(strSubject, "&", "&amp;");
        CCgi.replace_character(strSubject, "\"", "&quot;");
        CCgi.replace_character(strEntry, "'", "&#039;");

        // If the checkbox is checked, the value will be one; otherwise take it
        // as false.
        if (strAllowComments == "1")
        {
            strAllowComments = "TRUE";
        }
        else
        {
            strAllowComments = "FALSE";
        }

        if (!strBlogId.size())
        {
            sprintf(sql,
                "INSERT INTO blog_entry "
                "(posttime, updatetime, ip, body, "
                "title, allow_comments) "
                "VALUES "
                "(EXTRACT(EPOCH FROM NOW())::bigint, EXTRACT(EPOCH FROM NOW())::bigint, '%s', '%s', "
                "'%s', %s)",
                getenv("REMOTE_ADDR"), strEntry.c_str(),
                strSubject.c_str(), strAllowComments.c_str());
        }
        else
        {
            sprintf(sql,
                "UPDATE blog_entry "
                "SET body = '%s', "
                "    title = '%s', "
                "    ip = '%s', "
                "    allow_comments = %s, "
                "    updatetime = EXTRACT(EPOCH FROM NOW())::bigint "
                "WHERE id = %s",
                strEntry.c_str(),
                strSubject.c_str(),
                getenv("REMOTE_ADDR"),
                strAllowComments.c_str(),
                strBlogId.c_str());
        }

        try
        {
            pqxx::work insert(db, "NewEntryTransaction");
            insert.exec(sql);
            insert.commit();
        }
        catch (const PGSTD::exception& e)
        {
            CFormInput.set_error("general", e.what());
            return;
        }

        CLayout.header(admin_location.c_str(), true);

        exit(0);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int main(void)
{
    // GET variable, specifies a single entry id to display.
    std::string strBlogId;

    blog_layout::Layout CLayout;
    blog_cgi::Cgi CCgi;
    form_input::FormInput CFormInput("<span id=\"error\">", "</span>");

    try
    {
        pqxx::connection db(db_dsn);

        CLayout.fetch_settings(db);

        // Check to see if the form has been submitted.
        if (getenv("CONTENT_LENGTH") != NULL)
        {
            handle_form_input(db, CCgi, CLayout, CFormInput);
        }
        // If GET variables are present, look for an entry id.
        else if (getenv("QUERY_STRING"))
        {
            CCgi.parse_get();

            strBlogId = CCgi.get_get("eid");
        }

        CLayout.print_page_header(" - Create New Entry");

        print_main(db, CCgi, CLayout, CFormInput, strBlogId);

        CLayout.print_sidebar(db, true);
    }
    catch (const PGSTD::exception& e)
    {
        std::cout << "<p><strong>Error</strong> " << e.what() << "</p>" << std::endl;
    }

    CLayout.print_page_footer();

    return 0;
}
