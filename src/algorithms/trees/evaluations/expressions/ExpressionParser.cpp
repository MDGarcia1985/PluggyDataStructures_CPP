/*
 * File: ExpressionParser.cpp
 * Description: Implements recursive-descent parsing for arithmetic expression trees.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/evaluations/expressions/ExpressionParser.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace expressions
    {
        namespace
        {
            /*
             * Purpose: Convert common pasted math symbols into parser-supported ASCII operators.
             * Design: Scans UTF-8 byte patterns directly so the rest of the parser can stay ASCII-only.
             * Workflow: Walk the expression, translate known multiplication/minus variants, and copy all other bytes.
             * Data Handoff: Receives raw user expression text and returns normalized text for token parsing.
             */
            std::string normalizeExpressionText(const std::string& expression)
            {
                std::string normalized;
                for (std::size_t index = 0; index < expression.size();)
                {
                    const unsigned char first = static_cast<unsigned char>(expression[index]);

                    if (first == 0xE2 && index + 2 < expression.size())
                    {
                        const unsigned char second = static_cast<unsigned char>(expression[index + 1]);
                        const unsigned char third = static_cast<unsigned char>(expression[index + 2]);
                        if (second == 0x88 && third == 0x97)
                        {
                            normalized += '*';
                            index += 3;
                            continue;
                        }
                        if (second == 0x88 && third == 0x92)
                        {
                            normalized += '-';
                            index += 3;
                            continue;
                        }
                        if (second == 0x80 && (third == 0x93 || third == 0x94))
                        {
                            normalized += '-';
                            index += 3;
                            continue;
                        }
                    }

                    if (first == 0xC3 && index + 1 < expression.size())
                    {
                        const unsigned char second = static_cast<unsigned char>(expression[index + 1]);
                        if (second == 0x97)
                        {
                            normalized += '*';
                            index += 2;
                            continue;
                        }
                    }

                    normalized += expression[index];
                    ++index;
                }
                return normalized;
            }
        }

        /*
         * Purpose: Initialize a parser for one expression.
         * Design: Normalizes pasted operators before parsing so grammar logic only handles ASCII tokens.
         * Workflow: Store normalized text, start at position zero, and clear any previous error state.
         * Data Handoff: Receives user expression text and prepares parser-owned scanning state.
         */
        ExpressionParser::ExpressionParser(std::string expression)
            : text_(normalizeExpressionText(expression)), position_(0), error_()
        {
        }

        /*
         * Purpose: Advance past whitespace between tokens.
         * Design: Treats whitespace as insignificant everywhere the grammar expects a token boundary.
         * Workflow: Move the cursor forward while the current byte is classified as space.
         * Data Handoff: Mutates the parser cursor without producing a syntax node.
         */
        void ExpressionParser::skipSpaces()
        {
            while (position_ < text_.size() &&
                std::isspace(static_cast<unsigned char>(text_[position_])))
            {
                ++position_;
            }
        }

        /*
         * Purpose: Consume one expected punctuation character when it is present.
         * Design: Gives parenthesis parsing a small reusable primitive with whitespace handling.
         * Workflow: Skip spaces, compare the current character, and advance only on a match.
         * Data Handoff: Mutates the parser cursor and returns whether the expected token was found.
         */
        bool ExpressionParser::match(char expected)
        {
            skipSpaces();
            if (position_ < text_.size() && text_[position_] == expected)
            {
                ++position_;
                return true;
            }
            return false;
        }

        /*
         * Purpose: Parse the complete expression and reject unconsumed trailing input.
         * Design: Uses recursive descent from the lowest-precedence grammar rule.
         * Workflow: Parse an expression subtree, skip final spaces, and report unexpected leftovers.
         * Data Handoff: Returns the owned expression-tree root or nullptr with error_ populated.
         */
        std::unique_ptr<ExpressionNode> ExpressionParser::parse()
        {
            std::unique_ptr<ExpressionNode> root = parseExpression();
            if (root == nullptr)
            {
                return nullptr;
            }

            skipSpaces();
            if (position_ != text_.size())
            {
                error_ = "Unexpected token near '" + text_.substr(position_) + "'.";
                return nullptr;
            }

            return root;
        }

        /*
         * Purpose: Parse addition and subtraction expressions.
         * Design: Implements the lowest-precedence binary layer as left-associative reductions.
         * Workflow: Parse a term, then fold following plus/minus terms into parent operator nodes.
         * Data Handoff: Returns an owned subtree representing all consumed additive operations.
         */
        std::unique_ptr<ExpressionNode> ExpressionParser::parseExpression()
        {
            std::unique_ptr<ExpressionNode> node = parseTerm();
            while (node != nullptr)
            {
                skipSpaces();
                if (position_ >= text_.size() || (text_[position_] != '+' && text_[position_] != '-'))
                {
                    break;
                }

                const char op = text_[position_++];
                std::unique_ptr<ExpressionNode> right = parseTerm();
                if (right == nullptr)
                {
                    return nullptr;
                }

                node = makeExpressionBinaryNode(op, std::move(node), std::move(right));
            }
            return node;
        }

        /*
         * Purpose: Parse multiplication and division expressions.
         * Design: Keeps multiplicative precedence higher than addition by sitting below parseExpression().
         * Workflow: Parse a factor, then fold following multiply/divide factors into parent operator nodes.
         * Data Handoff: Returns an owned subtree representing all consumed multiplicative operations.
         */
        std::unique_ptr<ExpressionNode> ExpressionParser::parseTerm()
        {
            std::unique_ptr<ExpressionNode> node = parseFactor();
            while (node != nullptr)
            {
                skipSpaces();
                if (position_ >= text_.size() || (text_[position_] != '*' && text_[position_] != '/'))
                {
                    break;
                }

                const char op = text_[position_++];
                std::unique_ptr<ExpressionNode> right = parseFactor();
                if (right == nullptr)
                {
                    return nullptr;
                }

                node = makeExpressionBinaryNode(op, std::move(node), std::move(right));
            }
            return node;
        }

        /*
         * Purpose: Parse the atomic units of the grammar.
         * Design: Supports parenthesized expressions, unary signs, and numeric literals.
         * Workflow: Check for grouping, then unary operators, and finally fall back to number parsing.
         * Data Handoff: Returns an owned factor subtree or nullptr with error_ describing the syntax issue.
         */
        std::unique_ptr<ExpressionNode> ExpressionParser::parseFactor()
        {
            skipSpaces();
            if (position_ >= text_.size())
            {
                error_ = "Expected a number or parenthesized expression.";
                return nullptr;
            }

            if (match('('))
            {
                std::unique_ptr<ExpressionNode> node = parseExpression();
                if (node == nullptr)
                {
                    return nullptr;
                }
                if (!match(')'))
                {
                    error_ = "Expected a closing parenthesis.";
                    return nullptr;
                }
                return node;
            }

            if (text_[position_] == '+' || text_[position_] == '-')
            {
                const char sign = text_[position_++];
                std::unique_ptr<ExpressionNode> factor = parseFactor();
                if (factor == nullptr)
                {
                    return nullptr;
                }
                if (sign == '+')
                {
                    return factor;
                }

                std::unique_ptr<ExpressionNode> zero(new ExpressionNode("0"));
                return makeExpressionBinaryNode('-', std::move(zero), std::move(factor));
            }

            return parseNumber();
        }

        /*
         * Purpose: Parse an integer or decimal numeric literal.
         * Design: Accepts digits with at most one decimal point and leaves validation of range to stod().
         * Workflow: Scan digits and one optional dot, require at least one digit, and create a leaf node.
         * Data Handoff: Returns an owned numeric leaf or nullptr with error_ set to a number expectation.
         */
        std::unique_ptr<ExpressionNode> ExpressionParser::parseNumber()
        {
            skipSpaces();
            const std::size_t start = position_;
            bool sawDigit = false;
            bool sawDot = false;

            while (position_ < text_.size())
            {
                const char current = text_[position_];
                if (std::isdigit(static_cast<unsigned char>(current)))
                {
                    sawDigit = true;
                    ++position_;
                    continue;
                }
                if (current == '.' && !sawDot)
                {
                    sawDot = true;
                    ++position_;
                    continue;
                }
                break;
            }

            if (!sawDigit)
            {
                error_ = "Expected a number.";
                return nullptr;
            }

            return std::unique_ptr<ExpressionNode>(new ExpressionNode(text_.substr(start, position_ - start)));
        }

        const std::string& ExpressionParser::error() const
        {
            return error_;
        }
    }
}
