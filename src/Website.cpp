/*
 * File: Website.cpp
 * Description: Implements website storage, display formatting, and case-insensitive matching.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "Website.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Create an empty website object.
     * Design: Keeps default construction safe by storing empty strings instead of null values.
     * Workflow: Initialize the name and URL members before the object is used.
     * Data Handoff: Produces a Website that other classes can fill later with setters.
     */
    Website::Website()
        : name_(""), url_("")
    {
    }

    /*
     * Purpose: Create a website object from a name and URL.
     * Design: Accepts strings by value so callers can pass literals, copies, or movable strings.
     * Workflow: Move the incoming values into the private member variables.
     * Data Handoff: Stores validated-by-caller text for later display, search, and list storage.
     */
    Website::Website(std::string name, std::string url)
        : name_(std::move(name)), url_(std::move(url))
    {
    }

    /*
     * Purpose: Read the website name without modifying the object.
     * Design: Returns a const reference to avoid copying the stored string.
     * Workflow: Hand back direct read-only access to name_.
     * Data Handoff: Gives display, search, and test code the current website name.
     */
    const std::string& Website::name() const
    {
        return name_;
    }

    /*
     * Purpose: Read the website URL without modifying the object.
     * Design: Returns a const reference to avoid copying the stored string.
     * Workflow: Hand back direct read-only access to url_.
     * Data Handoff: Gives display, search, and test code the current website URL.
     */
    const std::string& Website::url() const
    {
        return url_;
    }

    /*
     * Purpose: Replace the stored website name.
     * Design: Accepts a const reference because the function only needs to copy from the input.
     * Workflow: Assign the incoming text to the private name_ member.
     * Data Handoff: Takes user- or file-provided text and stores it inside this Website.
     */
    void Website::setName(const std::string& name)
    {
        name_ = name;
    }

    /*
     * Purpose: Replace the stored website URL.
     * Design: Accepts a const reference because the function only needs to copy from the input.
     * Workflow: Assign the incoming text to the private url_ member.
     * Data Handoff: Takes user- or file-provided text and stores it inside this Website.
     */
    void Website::setUrl(const std::string& url)
    {
        url_ = url;
    }

    /*
     * Purpose: Decide whether a search term appears in the website name or URL.
     * Design: Converts all compared strings to lowercase so matching is case-insensitive.
     * Workflow: Lowercase the name, URL, and search term, then look for the term in both fields.
     * Data Handoff: Returns true or false to WebsiteList so it can collect matching results.
     */
    bool Website::matches(const std::string& searchTerm) const
    {
        const std::string lowerName = toLowerCopy(name_);
        const std::string lowerUrl = toLowerCopy(url_);
        const std::string lowerSearch = toLowerCopy(searchTerm);

        return lowerName.find(lowerSearch) != std::string::npos ||
               lowerUrl.find(lowerSearch) != std::string::npos;
    }

    /*
     * Purpose: Format a website as a single line for console output.
     * Design: Keeps display text creation close to the data it represents.
     * Workflow: Join the stored name and URL with a readable separator.
     * Data Handoff: Returns text used by Display when printing websites.
     */
    std::string Website::toDisplayString() const
    {
        return name_ + " - " + url_;
    }

    /*
     * Purpose: Create a lowercase copy of text for case-insensitive comparisons.
     * Design: Works on a copy so the original string stored by the caller is not changed.
     * Workflow: Transform each character with std::tolower and return the converted string.
     * Data Handoff: Supplies normalized text to matches().
     */
    std::string Website::toLowerCopy(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            });

        return text;
    }
}
