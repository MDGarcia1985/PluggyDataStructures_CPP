/*
 * File: ExpressionNode.h
 * Description: Declares the internal node shape used by expression-tree evaluation.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"

#include <memory>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace expressions
    {
        struct ExpressionNode
        {
            std::string token;
            std::unique_ptr<ExpressionNode> left;
            std::unique_ptr<ExpressionNode> right;

            explicit ExpressionNode(std::string tokenText);
        };

        bool isExpressionOperator(const std::string& token);
        std::unique_ptr<ExpressionNode> makeExpressionBinaryNode(
            char op,
            std::unique_ptr<ExpressionNode> left,
            std::unique_ptr<ExpressionNode> right);
    }
}
