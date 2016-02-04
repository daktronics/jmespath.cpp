/****************************************************************************
**
** Author: Róbert Márki <gsmiko@gmail.com>
** Copyright (c) 2016 Róbert Márki
**
** This file is part of the jmespath.cpp project which is distributed under
** the MIT License (MIT).
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include "jmespath/interpreter/expressionevaluator.h"
#include "jmespath/ast/allnodes.h"
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

namespace jmespath { namespace interpreter {

namespace rng = boost::range;

ExpressionEvaluator::ExpressionEvaluator()
    : AbstractVisitor()
{
}

ExpressionEvaluator::ExpressionEvaluator(const Json &contextValue)
    : AbstractVisitor()
{
    setContext(contextValue);
}

void ExpressionEvaluator::setContext(const Json &value)
{
    m_context = value;
}

Json ExpressionEvaluator::currentContext() const
{
    return m_context;
}

void ExpressionEvaluator::evaluateProjection(ast::ExpressionNode *expression)
{
    Json contextArray = m_context;
    Json result;
    if (contextArray.is_array())
    {
        result = Json(Json::value_t::array);
        for (auto item: contextArray)
        {
            m_context = item;
            visit(expression);
            if (!m_context.is_null())
            {
                result.push_back(m_context);
            }
        }
    }
    m_context = result;
}

void ExpressionEvaluator::visit(ast::AbstractNode *node)
{
    node->accept(this);
}

void ExpressionEvaluator::visit(ast::ExpressionNode *node)
{
    node->accept(this);
}

void ExpressionEvaluator::visit(ast::IdentifierNode *node)
{
    Json result;
    if (m_context.is_object())
    {
        result = m_context[node->identifier];
    }
    m_context = result;
}

void ExpressionEvaluator::visit(ast::RawStringNode *node)
{
    m_context = node->rawString;
}

void ExpressionEvaluator::visit(ast::LiteralNode *node)
{
    m_context = Json::parse(node->literal);
}

void ExpressionEvaluator::visit(ast::SubexpressionNode *node)
{
    node->accept(this);
}

void ExpressionEvaluator::visit(ast::IndexExpressionNode *node)
{
    visit(&node->leftExpression);
    visit(&node->bracketSpecifier);
    if (node->isProjection())
    {
        evaluateProjection(&node->rightExpression);
    }
}

void ExpressionEvaluator::visit(ast::ArrayItemNode *node)
{
    Json result;
    if (m_context.is_array())
    {
        int arrayIndex = node->index;
        if (arrayIndex < 0)
        {
            arrayIndex = m_context.size() + arrayIndex;
        }
        if ((arrayIndex >= 0 ) && (arrayIndex < m_context.size()))
        {
            result = m_context[arrayIndex];
        }
    }
    m_context = result;
}

void ExpressionEvaluator::visit(ast::FlattenOperatorNode *node)
{
    Json result;
    Json contextArray = m_context;
    if (contextArray.is_array())
    {
        Json arrayValue(Json::value_t::array);
        for (auto const& item: contextArray)
        {
            if (item.is_array())
            {
                rng::copy(item, std::back_inserter(arrayValue));
            }
            else
            {
                arrayValue.push_back(item);
            }
        }
        result = arrayValue;
    }
    m_context = result;
}

void ExpressionEvaluator::visit(ast::BracketSpecifierNode *node)
{
    node->accept(this);
}
}} // namespace jmespath::interpreter
