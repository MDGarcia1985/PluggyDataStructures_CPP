/*
 * File: ExpressionEvaluation.h
 * Description: Declares expression-tree parsing, traversal, and evaluation helpers.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetTree.h"

#include <string>


// named container pds = Pluggy Data Structures
namespace pds
{
    struct ExpressionEvaluationResult
    {
        bool success;
        double value;
        std::string error;
        std::vector<std::string> prefix;
        std::vector<std::string> infix;
        std::vector<std::string> postfix;
        std::vector<std::string> levelOrder;
    };

    /*
     * Purpose: Parse an arithmetic expression into a binary expression tree and evaluate it.
     * Design: Keeps expression semantics separate from TargetTree, which remains a BST of Targets.
     * Workflow: Tokenize, build a precedence-aware tree, traverse it, and fold it to a number.
     * Data Handoff: Receives expression text and returns result, diagnostics, and traversal snapshots.
     */
    ExpressionEvaluationResult evaluateExpression(const std::string& expression);

    /*
     * Purpose: Preserve the old TargetTree overload while expression-tree semantics live elsewhere.
     * Design: Makes accidental calls explicit instead of pretending a Target BST is an expression tree.
     * Workflow: Ignore the TargetTree and return a clear guidance string.
     * Data Handoff: Receives a TargetTree reference and returns a compatibility message.
     */
    std::string evaluateExpression(const TargetTree& tree);
}
