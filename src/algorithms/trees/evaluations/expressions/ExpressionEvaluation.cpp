/*
 * File: ExpressionEvaluation.cpp
 * Description: Implements expression-tree traversal and evaluation orchestration.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/ExpressionEvaluation.h"

#include "algorithms/trees/evaluations/expressions/ExpressionParser.h"

#include <cmath>
#include <queue>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        using expressions::ExpressionNode;

        /*
         * Purpose: Collect root-left-right traversal tokens from an expression tree.
         * Design: Uses the standard pre-order recursion over the privately owned node shape.
         * Workflow: Visit the current token, then recurse into the left and right subtrees.
         * Data Handoff: Appends token text into the caller-owned output vector.
         */
        void collectPrefix(const ExpressionNode* node, std::vector<std::string>& output)
        {
            if (node == nullptr)
            {
                return;
            }
            output.push_back(node->token);
            collectPrefix(node->left.get(), output);
            collectPrefix(node->right.get(), output);
        }

        /*
         * Purpose: Collect left-root-right traversal tokens from an expression tree.
         * Design: Adds parentheses around operator subtrees so the displayed infix order is unambiguous.
         * Workflow: Optionally open a group, visit left subtree, current token, right subtree, then close.
         * Data Handoff: Appends token and parenthesis text into the caller-owned output vector.
         */
        void collectInfix(const ExpressionNode* node, std::vector<std::string>& output)
        {
            if (node == nullptr)
            {
                return;
            }

            const bool parenthesize = expressions::isExpressionOperator(node->token);
            if (parenthesize)
            {
                output.push_back("(");
            }
            collectInfix(node->left.get(), output);
            output.push_back(node->token);
            collectInfix(node->right.get(), output);
            if (parenthesize)
            {
                output.push_back(")");
            }
        }

        /*
         * Purpose: Collect left-right-root traversal tokens from an expression tree.
         * Design: Uses the standard post-order recursion useful for stack-based expression evaluation demos.
         * Workflow: Recurse into children first, then append the current token.
         * Data Handoff: Appends token text into the caller-owned output vector.
         */
        void collectPostfix(const ExpressionNode* node, std::vector<std::string>& output)
        {
            if (node == nullptr)
            {
                return;
            }
            collectPostfix(node->left.get(), output);
            collectPostfix(node->right.get(), output);
            output.push_back(node->token);
        }

        /*
         * Purpose: Collect breadth-first traversal tokens from an expression tree.
         * Design: Uses a queue so each tree level is visited before the next.
         * Workflow: Enqueue the root, repeatedly visit the front node, and enqueue its children.
         * Data Handoff: Appends token text into the caller-owned output vector.
         */
        void collectLevelOrder(const ExpressionNode* root, std::vector<std::string>& output)
        {
            std::queue<const ExpressionNode*> pending;
            if (root != nullptr)
            {
                pending.push(root);
            }

            while (!pending.empty())
            {
                const ExpressionNode* node = pending.front();
                pending.pop();
                output.push_back(node->token);

                if (node->left != nullptr)
                {
                    pending.push(node->left.get());
                }
                if (node->right != nullptr)
                {
                    pending.push(node->right.get());
                }
            }
        }

        /*
         * Purpose: Recursively evaluate an expression tree into one numeric value.
         * Design: Treats leaves as numbers and interior operator nodes as binary arithmetic.
         * Workflow: Convert leaves with stod, fold child values through the operator, and guard division by zero.
         * Data Handoff: Writes the computed value or an error message through output parameters.
         */
        bool evaluateNode(const ExpressionNode* node, double& value, std::string& error)
        {
            if (node == nullptr)
            {
                error = "Expression tree is empty.";
                return false;
            }

            if (!expressions::isExpressionOperator(node->token))
            {
                try
                {
                    std::size_t consumed = 0;
                    value = std::stod(node->token, &consumed);
                    if (consumed != node->token.size())
                    {
                        error = "Invalid number '" + node->token + "'.";
                        return false;
                    }
                    return true;
                }
                catch (const std::exception&)
                {
                    error = "Invalid number '" + node->token + "'.";
                    return false;
                }
            }

            double leftValue = 0.0;
            double rightValue = 0.0;
            if (!evaluateNode(node->left.get(), leftValue, error) ||
                !evaluateNode(node->right.get(), rightValue, error))
            {
                return false;
            }

            if (node->token == "+")
            {
                value = leftValue + rightValue;
                return true;
            }
            if (node->token == "-")
            {
                value = leftValue - rightValue;
                return true;
            }
            if (node->token == "*")
            {
                value = leftValue * rightValue;
                return true;
            }
            if (std::fabs(rightValue) < 0.000000000001)
            {
                error = "Division by zero.";
                return false;
            }
            value = leftValue / rightValue;
            return true;
        }
    }

    /*
     * Purpose: Parse an arithmetic expression into a binary tree and evaluate it.
     * Design: Uses recursive descent so parentheses and operator precedence shape the tree.
     * Workflow: Build the tree, collect standard traversals, and recursively fold operators.
     * Data Handoff: Returns traversal tokens, numeric result, or a clear parse/evaluation error.
     */
    ExpressionEvaluationResult evaluateExpression(const std::string& expression)
    {
        ExpressionEvaluationResult result{false, 0.0, "", {}, {}, {}, {}};

        expressions::ExpressionParser parser(expression);
        std::unique_ptr<ExpressionNode> root = parser.parse();
        if (root == nullptr)
        {
            result.error = parser.error().empty() ? "Expression could not be parsed." : parser.error();
            return result;
        }

        collectPrefix(root.get(), result.prefix);
        collectInfix(root.get(), result.infix);
        collectPostfix(root.get(), result.postfix);
        collectLevelOrder(root.get(), result.levelOrder);

        if (!evaluateNode(root.get(), result.value, result.error))
        {
            return result;
        }

        result.success = true;
        result.error.clear();
        return result;
    }

    /*
     * Purpose: Preserve source compatibility with the old placeholder overload.
     * Design: Keeps TargetTree as a Target BST and directs callers to the string evaluator.
     * Workflow: Ignore the tree and return a guidance message.
     * Data Handoff: Returns a short explanatory string to legacy callers.
     */
    std::string evaluateExpression(const TargetTree& tree)
    {
        (void)tree;
        return "expression evaluation expects expression text, not a TargetTree";
    }
}
