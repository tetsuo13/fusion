/**
 * Fusion. Various JavaScript functions used by the admin screen.
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

// ----------------------------------------------------------------------------
// Using the existing text from the textarea input, spawn a popup and display
// that text as it would appear on the web site.
//
// @param object entryInput - The textarea input object.
// @return void
// ----------------------------------------------------------------------------
function preview(entryInput)
{
    var hWindow = window.open("", "Preview", "height=480,width=640,toolbars=no,location=no,menubar=no,status=yes,resizable=yes,scrollbars=yes");

    hWindow.document.writeln("<html>");
    hWindow.document.writeln("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
    hWindow.document.writeln("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
    hWindow.document.writeln("<head>");
    hWindow.document.writeln("<title>fusion - Create New Entry</title>");
    hWindow.document.writeln("<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />");
    hWindow.document.writeln("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen, projection\" href=\"http://www.example.com/blog/content.css\" />");
    hWindow.document.writeln("</head>");
    hWindow.document.writeln("<body>");
    hWindow.document.writeln("<div id=\"main\"><div id=\"main2\"><div id=\"main3\">");
    hWindow.document.writeln("<div class=\"post-body\">");
    hWindow.document.writeln(entryInput.value);
    hWindow.document.writeln("</div>");
    hWindow.document.writeln("</div></div></div>");
    hWindow.document.writeln("</body>");
    hWindow.document.writeln("</html>");

    hWindow.focus();
}

// ----------------------------------------------------------------------------
// Appends a matching opening and closing tag to the entry textarea input.
//
// @param string tag - The tag name: p, a, ul, img, etc.
// @param string attribute - Any attributes to give the tag, ie 'class'.
// @return void
// ----------------------------------------------------------------------------
function addTag(tag, attribute)
{
    var entryInput = document.forms[0].entry;
    var inputHtml = "<" + tag;

    if (attribute.length > 0)
    {
        attribute = attribute.replace(/'/g, '"');

        inputHtml += " " + attribute;
    }

    // Not all tags require a matching ending tag.
    switch (tag)
    {
    case "img":
        inputHtml += " />";
        break;

    default:
        inputHtml += "></" + tag + ">";
        break;
    }

    entryInput.focus();

    entryInput.value += inputHtml + "\n";
}
