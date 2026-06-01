/*
 * File: Target.cpp
 * Description: Implements generic record storage, display formatting, and case-insensitive matching.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "Target.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Create an empty target object.
     * Design: Keeps default construction safe by storing empty strings instead of null values.
     * Workflow: Initialize both generic fields before the object is used.
     * Data Handoff: Produces a Target that other classes can fill later with setters.
     */
    Target::Target()
        : fieldOne_(""), fieldTwo_("")
    {
    }

    /*
     * Purpose: Create a target object from two generic record fields.
     * Design: Accepts strings by value so callers can pass literals, copies, or movable strings.
     * Workflow: Move the incoming values into the private member variables.
     * Data Handoff: Stores validated-by-caller text for later display, search, and list storage.
     */
    Target::Target(std::string fieldOne, std::string fieldTwo)
        : fieldOne_(std::move(fieldOne)), fieldTwo_(std::move(fieldTwo))
    {
    }

    /*
     * Purpose: Read the first generic field without modifying the object.
     * Design: Returns a const reference to avoid copying the stored string.
     * Workflow: Hand back direct read-only access to fieldOne_.
     * Data Handoff: Gives display, search, and test code the first field value.
     */
    const std::string& Target::fieldOne() const
    {
        return fieldOne_;
    }

    /*
     * Purpose: Read the second generic field without modifying the object.
     * Design: Returns a const reference to avoid copying the stored string.
     * Workflow: Hand back direct read-only access to fieldTwo_.
     * Data Handoff: Gives display, search, and test code the second field value.
     */
    const std::string& Target::fieldTwo() const
    {
        return fieldTwo_;
    }

    /*
     * Purpose: Replace the first generic field.
     * Design: Accepts a const reference because the function only needs to copy from the input.
     * Workflow: Assign the incoming text to the private fieldOne_ member.
     * Data Handoff: Takes user- or file-provided text and stores it inside this Target.
     */
    void Target::setFieldOne(const std::string& fieldOne)
    {
        fieldOne_ = fieldOne;
    }

    /*
     * Purpose: Replace the second generic field.
     * Design: Accepts a const reference because the function only needs to copy from the input.
     * Workflow: Assign the incoming text to the private fieldTwo_ member.
     * Data Handoff: Takes user- or file-provided text and stores it inside this Target.
     */
    void Target::setFieldTwo(const std::string& fieldTwo)
    {
        fieldTwo_ = fieldTwo;
    }

    /*
     * Purpose: Decide whether a search term appears in either target field.
     * Design: Converts all compared strings to lowercase so matching is case-insensitive.
     * Workflow: Lowercase both fields and the search term, then look for the term in either field.
     * Data Handoff: Returns true or false to TargetList so it can collect matching results.
     */
    bool Target::matches(const std::string& searchTerm) const
    {
        const std::string lowerFieldOne = toLowerCopy(fieldOne_);
        const std::string lowerFieldTwo = toLowerCopy(fieldTwo_);
        const std::string lowerSearch = toLowerCopy(searchTerm);

        return lowerFieldOne.find(lowerSearch) != std::string::npos ||
               lowerFieldTwo.find(lowerSearch) != std::string::npos;
    }

    /*
     * Purpose: Format a target as a single line for console output.
     * Design: Keeps display text creation close to the data it represents.
     * Workflow: Join the two stored fields with a readable separator.
     * Data Handoff: Returns text used by Display when printing targets.
     */
    std::string Target::toDisplayString() const
    {
        return fieldOne_ + " - " + fieldTwo_;
    }

    /*
     * Purpose: Create a lowercase copy of text for case-insensitive comparisons.
     * Design: Works on a copy so the original string stored by the caller is not changed.
     * Workflow: Transform each character with std::tolower and return the converted string.
     * Data Handoff: Supplies normalized text to matches().
     */
    std::string Target::toLowerCopy(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            });

        return text;
    }
}
