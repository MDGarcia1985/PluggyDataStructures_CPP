/*
 * File: ExpressionParser.h
 * Description: Declares the internal recursive-descent parser for expression trees.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "algorithms/trees/evaluations/expressions/ExpressionNode.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace expressions
    {
        class ExpressionParser
        {
        private:
            std::string text_;
            std::size_t position_;
            std::string error_;

            void skipSpaces();
            bool match(char expected);
            std::unique_ptr<ExpressionNode> parseExpression();
            std::unique_ptr<ExpressionNode> parseTerm();
            std::unique_ptr<ExpressionNode> parseFactor();
            std::unique_ptr<ExpressionNode> parseNumber();

        public:
            explicit ExpressionParser(std::string expression);

            std::unique_ptr<ExpressionNode> parse();
            const std::string& error() const;
        };
    }
}
