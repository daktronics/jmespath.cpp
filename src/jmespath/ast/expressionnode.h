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
#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H
#include "jmespath/ast/abstractnode.h"
#include "jmespath/ast/variantnode.h"
#include <boost/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace jmespath { namespace ast {

class IdentifierNode;
class RawStringNode;
class LiteralNode;
class SubexpressionNode;
class IndexExpressionNode;
class HashWildcardNode;
class MultiselectListNode;
class MultiselectHashNode;
class NotExpressionNode;
class ComparatorExpressionNode;
class OrExpressionNode;
class AndExpressionNode;
class ParenExpressionNode;
class PipeExpressionNode;
/**
 * @brief The ExpressionNode class represents a JMESPath expression.
 */
class ExpressionNode : public VariantNode<
        boost::recursive_wrapper<IdentifierNode>,
        boost::recursive_wrapper<RawStringNode>,
        boost::recursive_wrapper<LiteralNode>,
        boost::recursive_wrapper<SubexpressionNode>,
        boost::recursive_wrapper<IndexExpressionNode>,
        boost::recursive_wrapper<HashWildcardNode>,
        boost::recursive_wrapper<MultiselectListNode>,
        boost::recursive_wrapper<MultiselectHashNode>,
        boost::recursive_wrapper<NotExpressionNode>,
        boost::recursive_wrapper<ComparatorExpressionNode>,
        boost::recursive_wrapper<OrExpressionNode>,
        boost::recursive_wrapper<AndExpressionNode>,
        boost::recursive_wrapper<ParenExpressionNode>,
        boost::recursive_wrapper<PipeExpressionNode> >
{
public:
    /**
     * @brief Constructs an empty ExpressionNode object
     */
    ExpressionNode();
    /**
     * @brief Constructs an ExpressionNode object with its child expression
     * initialized to \a expression
     * @param expression The node's child expression
     */
    ExpressionNode(const ValueType& expression);
    /**
     * @brief Assigns the @a other object to this object.
     * @param other An ExpressionNode object.
     * @return Returns a reference to this object.
     */
    ExpressionNode& operator=(const ExpressionNode& other);
    /**
     * @brief Assigns the @a other Expression to this object's expression.
     * @param expression An Expression object.
     * @return Returns a reference to this object.
     */
    ExpressionNode& operator=(const ValueType& expression);
};
}} // namespace jmespath::ast

BOOST_FUSION_ADAPT_STRUCT(
    jmespath::ast::ExpressionNode,
    (jmespath::ast::ExpressionNode::ValueType, value)
)
#endif // EXPRESSIONNODE_H
