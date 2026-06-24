/*
 * File: ExpressionNode.cpp
 * Description: Implements expression-tree node helpers.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/evaluations/expressions/ExpressionNode.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace expressions
    {
        ExpressionNode::ExpressionNode(std::string tokenText)
            : token(std::move(tokenText)), left(nullptr), right(nullptr)
        {
        }

        bool isExpressionOperator(const std::string& token)
        {
            return token == "+" || token == "-" || token == "*" || token == "/";
        }

        /*
         * Purpose: Create a binary operator node with the supplied children.
         * Design: Centralizes ownership transfer for left-associative parser reductions.
         * Workflow: Allocate the parent node, move in left and right child subtrees, and return the parent.
         * Data Handoff: Receives owned child nodes and returns one owned operator subtree.
         */
        std::unique_ptr<ExpressionNode> makeExpressionBinaryNode(
            char op,
            std::unique_ptr<ExpressionNode> left,
            std::unique_ptr<ExpressionNode> right)
        {
            std::unique_ptr<ExpressionNode> parent(new ExpressionNode(std::string(1, op)));
            parent->left = std::move(left);
            parent->right = std::move(right);
            return parent;
        }
    }
}
