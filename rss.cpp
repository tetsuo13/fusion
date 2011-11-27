/**
 * Fusion. Grabs the last n blog entries and creates an XML document. Based on
 * a GET argument, it can create an Atom, RSS, or RDF formatted XML document.
 * If none are specified, it defaults to RSS.
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
#include "Cgi.h"
#include "Layout.h"


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_header(const std::string& strType)
{
    const char* title     = "fusion";
    const char* copyright = "Copyright (c) 2005, Andrei Nicholson";
    const char* email     = "andre@neo-anime.org";
    const char* name      = "Andrei Nicholson";

    std::cout << "Content-type: application/" << strType << "+xml; charset=UTF-8" << std::endl << std::endl;

    std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    if (strType == "atom")
    {
        std::cout << "<feed xmlns=\"http://www.w3.org/2005/Atom\">" << std::endl;
        std::cout << "\t<title type=\"text\">" << title << "</title>" << std::endl;
        std::cout << "\t<id>" << base_url << "</id>" << std::endl;
        std::cout << "\t<link rel=\"alternate\" type=\"text/html\" hreflang=\"en\" href=\"" << base_url << "\" />" << std::endl;
        std::cout << "\t<link rel=\"self\" type=\"application/atom+xml\" href=\"" << base_url << "feed.xml?type=atom\" />" << std::endl;

        std::cout << "\t<author>" << std::endl
                  << "\t\t<name>" << name << "</name>" << std::endl
                  << "\t\t<uri>" << base_url << "</uri>" << std::endl
                  << "\t\t<email>" << email << "</email>" << std::endl
                  << "\t</author>" << std::endl;

        std::cout << "\t<rights>" << copyright << "</rights>" << std::endl;
        std::cout << "\t<generator version=\"1.0\">" << name << "</generator>" << std::endl;
    }
    else if (strType == "rdf")
    {
        std::cout << "<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns=\"http://purl.org/rss/1.0/\">" << std::endl;
        std::cout << "\t<channel rdf:about=\"" << base_url << "feed.xml?type=rdf\">" << std::endl;
        std::cout << "\t\t<title>" << title << "</title>" << std::endl;
        std::cout << "\t\t<link>" << base_url << "</link>" << std::endl;
        std::cout << "\t\t<description>" << name << "</description>" << std::endl;
    }
    else if (strType == "rss")
    {
        std::cout << "<rss version=\"2.0\">" << std::endl;
        std::cout << "\t<channel>" << std::endl;
        std::cout << "\t\t<title>" << title << "</title>" << std::endl;
        std::cout << "\t\t<link>" << base_url << "</link>" << std::endl;
        std::cout << "\t\t<copyright>" << copyright << "</copyright>" << std::endl;
        std::cout << "\t\t<managingEditor>" << email << " (" << name << ")</managingEditor>" << std::endl;
        std::cout << "\t\t<description>" << name << "</description>" << std::endl;
        std::cout << "\t\t<generator>" << name << "</generator>" << std::endl;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_main(pqxx::connection& db, const std::string& strType)
{
    pqxx::work w(db);
    pqxx::result res;
    pqxx::result::size_type i = 0;

    blog_layout::Layout CLayout;

    // RFC 3339 timestamp.
    const char* rfc3339 = "%Y-%m-%dT%H:%M:%S%z";

    // RFC 822 timestamp.
    const char* rfc822 = "%a, %d %b %Y %H:%M:%S %Z";

    const char* sql = "SELECT id, posttime, updatetime, body, title "
                      "FROM blog_entry "
                      "ORDER BY posttime DESC "
                      "LIMIT 10";

    try
    {
        res = w.exec(sql);
        w.commit();
    }
    catch (const PGSTD::exception& e)
    {
        // If there are any problems, simply exit this function. The XML header
        // and footer will still be printed, creating a valid XML document.
        return;
    }

    if (strType == "rdf")
    {
        std::cout << "\t\t<items>" << std::endl
                  << "\t\t\t<rdf:Seq>" << std::endl;

        for (i = 0; i != res.size(); ++i)
        {
            std::cout << "\t\t\t\t<rdf:li resource=\"" << base_url << "/?eid=" << res[i]["id"].c_str() << "\" />" << std::endl;
        }

        std::cout << "\t\t\t</rdf:Seq>" << std::endl
                  << "\t\t</items>" << std::endl;
        std::cout << "\t</channel>" << std::endl;

        for (i = 0; i != res.size(); ++i)
        {
            std::cout << "\t<item rdf:about=\"" << base_url << "/?eid=" << res[i]["id"].c_str() << "\">" << std::endl;
            std::cout << "\t\t<title>" << res[i]["title"].c_str() << "</title>" << std::endl;
            std::cout << "\t\t<link>" << base_url << "/?eid=" << res[i]["id"].c_str() << "</link>" << std::endl;
            std::cout << "\t\t<description><![CDATA[" << res[i]["body"].c_str() << "]]></description>" << std::endl;
            std::cout << "\t</item>" << std::endl;
        }
    }
    else if (strType == "atom")
    {
        // The updated time for the feed is the posted time of the most recent
        // entry.
        std::cout << "\t<updated>" << CLayout.format_date(res[0]["posttime"].c_str(), rfc3339) << "</updated>" << std::endl;

        for (i = 0; i != res.size(); ++i)
        {
            std::cout << "\t<entry>" << std::endl;
            std::cout << "\t\t<title type=\"text\">" << res[i]["title"].c_str() << "</title>" << std::endl;
            std::cout << "\t\t<content type=\"xhtml\"><div xmlns=\"http://www.w3.org/1999/xhtml\">" << res[i]["body"].c_str() << "</div></content>" << std::endl;
            std::cout << "\t\t<id>" << base_url << "/?eid=" << res[i]["id"].c_str() << "</id>" << std::endl;

            std::cout << "\t\t<author>" << std::endl
                      << "\t\t\t<name>Andrei Nicholson</name>" << std::endl
                      << "\t\t</author>" << std::endl;

            std::cout << "\t\t<link rel=\"alternate\" href=\"" << base_url << "/?eid=" << res[i]["id"].c_str() << "\" />" << std::endl;
            std::cout << "\t\t<published>" << CLayout.format_date(res[i]["posttime"].c_str(), rfc3339) << "</published>" << std::endl;
            std::cout << "\t\t<updated>" << CLayout.format_date(res[i]["updatetime"].c_str(), rfc3339) << "</updated>" << std::endl;
            std::cout << "\t</entry>" << std::endl;
        }
    }
    else if (strType == "rss")
    {
        std::cout << "\t\t<pubDate>" << CLayout.format_date(res[0]["posttime"].c_str(), rfc822) << "</pubDate>" << std::endl;

        for (i = 0; i != res.size(); ++i)
        {
            std::cout << "\t\t<item>" << std::endl;
            std::cout << "\t\t\t<title>" << res[i]["title"].c_str() << "</title>" << std::endl;
            std::cout << "\t\t\t<link>" << base_url << "/?eid=" << res[i]["id"].c_str() << "</link>" << std::endl;
            std::cout << "\t\t\t<description><![CDATA[" << res[i]["body"].c_str() << "]]></description>" << std::endl;
            std::cout << "\t\t\t<pubDate>" << CLayout.format_date(res[i]["posttime"].c_str(), rfc822) << "</pubDate>" << std::endl;
            std::cout << "\t\t</item>" << std::endl;
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void print_footer(const std::string& strType)
{
    if (strType == "atom")
    {
        std::cout << "</feed>";
    }
    else if (strType == "rdf")
    {
        std::cout << "</rdf:RDF>";
    }
    else if (strType == "rss")
    {
        std::cout << "\t</channel>" << std::endl;
        std::cout << "</rss>";
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int main(void)
{
    blog_cgi::Cgi CCgi;
    std::string strType;

    try
    {
        pqxx::connection db(db_dsn);

        if (getenv("QUERY_STRING"))
        {
            CCgi.parse_get();

            strType = CCgi.get_get("type");

            // If the XML type doesn't match any that we're prepared to output,
            // default to RSS.
            if (strType != "rss" && strType != "atom" && strType != "rdf")
            {
                strType = "rss";
            }
        }

        print_header(strType);
        print_main(db, strType);
        print_footer(strType);
    }
    catch (const PGSTD::exception& e)
    {
        // Do nothing.
    }

    return 0;
}
