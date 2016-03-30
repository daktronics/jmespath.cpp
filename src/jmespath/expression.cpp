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
#include "jmespath/expression.h"
#include "jmespath/ast/allnodes.h"

namespace jmespath {

Expression::Expression()
{
}

Expression::Expression(const Expression &other)
{
    *this = other;
}

Expression::Expression(Expression &&other)
{
    *this = std::move(other);
}

Expression& Expression::operator=(const Expression &other)
{
    if (this != &other)
    {
        m_expressionString = other.m_expressionString;
        m_astRoot = other.m_astRoot;
    }
    return *this;
}

Expression& Expression::operator=(Expression &&other)
{
    if (this != &other)
    {
        m_expressionString = std::move(other.m_expressionString);
        m_astRoot = std::move(other.m_astRoot);
    }
    return *this;
}

detail::String Expression::toString() const
{
    return m_expressionString;
}

bool Expression::isEmpty() const
{
    return m_astRoot.isNull();
}

void Expression::parseExpression(const detail::String& expressionString)
{
    if (!expressionString.empty())
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
        static ParserType s_parser;
        m_astRoot = s_parser.parse(expressionString);
#pragma clang diagnostic pop
    }
}

Expression& Expression::operator=(const detail::String& expressionString)
{
    parseExpression(expressionString);
    m_expressionString = expressionString;
    return *this;
}

Expression& Expression::operator=(detail::String &&expressionString)
{
    parseExpression(expressionString);
    m_expressionString = std::move(expressionString);
    return *this;
}

bool Expression::operator==(const Expression &other) const
{
    if (this != &other)
    {
        return (m_expressionString == other.m_expressionString)
                && (m_astRoot == other.m_astRoot);
    }
    return true;
}
} // namespace jmespath
