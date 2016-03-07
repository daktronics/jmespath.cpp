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
#ifndef EXPRESSIONEVALUATOR_H
#define EXPRESSIONEVALUATOR_H
#include "jmespath/interpreter/abstractvisitor.h"
#include "jmespath/detail/types.h"
#include "jmespath/ast/expressionnode.h"
#include "jmespath/ast/functionexpressionnode.h"
#include <functional>
#include <tuple>
#include <unordered_map>
#include <boost/variant.hpp>

namespace jmespath { namespace interpreter {

using detail::Json;
using detail::String;
/**
 * @brief The ExpressionEvaluator class evaluates the AST structure.
 */
class ExpressionEvaluator : public AbstractVisitor
{
public:
    /**
     * @brief Constructs an ExpressionEvaluator object with the given
     * @a document as the context for the evaluation of the AST.
     * @param document JSON document on which the AST will be evaluated
     */
    ExpressionEvaluator(const Json& contextValue = {});
    /**
     * @brief Sets the context of the evaluation.
     * @param value JSON document to be used as the context.
     */
    void setContext(const Json& value);
    /**
     * @brief Returns the current evaluation context.
     * @return JSON document used as the context.
     */
    Json currentContext() const;
    /**
     * @brief Evaluates the projection of the given @a expression on the current
     * context.
     * @param expression The expression that gets projected.
     */
    virtual void evaluateProjection(ast::ExpressionNode* expression);

    void visit(ast::AbstractNode *node) override;
    void visit(ast::ExpressionNode *node) override;
    void visit(ast::IdentifierNode *node) override;
    void visit(ast::RawStringNode *node) override;
    void visit(ast::LiteralNode* node) override;
    void visit(ast::SubexpressionNode* node) override;
    void visit(ast::IndexExpressionNode* node) override;
    void visit(ast::ArrayItemNode* node) override;
    void visit(ast::FlattenOperatorNode*) override;
    void visit(ast::BracketSpecifierNode* node) override;
    void visit(ast::SliceExpressionNode* node) override;
    void visit(ast::ListWildcardNode* node) override;
    void visit(ast::HashWildcardNode* node) override;
    void visit(ast::MultiselectListNode* node) override;
    void visit(ast::MultiselectHashNode* node) override;
    void visit(ast::NotExpressionNode* node) override;
    void visit(ast::ComparatorExpressionNode* node) override;
    void visit(ast::OrExpressionNode* node) override;
    void visit(ast::AndExpressionNode* node) override;
    void visit(ast::ParenExpressionNode* node) override;
    void visit(ast::PipeExpressionNode* node) override;
    void visit(ast::CurrentNode*) override;
    void visit(ast::FilterExpressionNode* node) override;
    void visit(ast::FunctionExpressionNode* node) override;
    void visit(ast::ExpressionArgumentNode*) override;

private:
    /**
     * Type of the arguments in FunctionArgumentList.
     */
    using FunctionArgument
        = boost::variant<boost::blank, Json, ast::ExpressionNode>;
    /**
     * List of FunctionArgument objects.
     */
    using FunctionArgumentList = std::vector<FunctionArgument>;
    /**
     * Function wrapper type to which JMESPath built in function implementations
     * should conform to.
     */
    using Function = std::function<Json(FunctionArgumentList&)>;
    /**
     * The type of comparator functions used for comparing JSON values.
     */
    using JsonComparator = std::function<bool(const Json&, const Json&)>;
    /**
     * The type of comparator functions used for comparing size_t values
     */
    using SizeComparator = std::function<bool(const size_t&, const size_t&)>;
    /**
     * Describes a built in function implementation. The tuple's first item
     * stores the number of arguments expected by the function, the second
     * item is the comparator function used for comparing the actual number
     * of arguments with the expected argument count, while the third item
     * stores the callable functions wrapper.
     */
    using FunctionDescriptor = std::tuple<size_t, SizeComparator, Function>;
    /**
     * List of unevaluated function arguments.
     */
    using FunctionExpressionArgumentList
        = std::vector<ast::FunctionExpressionNode::ArgumentType>;
    /**
     * @brief Stores the evaluation context.
     */
    Json m_context;
    /**
     * @brief Maps the JMESPath built in function names to their
     * implementations.
     */
    std::unordered_map<String, FunctionDescriptor> m_functionMap;
    /**
     * @brief Adjust the value of the slice endpoint to make sure it's within
     * the array's bounds and points to the correct item.
     * @param length The length of the array that should be sliced.
     * @param endpoint The current value of the endpoint.
     * @param step The slice's step variable value.
     * @return Returns the endpoint's new value.
     */
    int adjustSliceEndpoint(int length, int endpoint, int step) const;
    /**
     * @brief Converts the @a json value to a boolean.
     * @param json The JSON value that needs to be converted.
     * @return Returns false if @a json is a false like value (false, 0, empty
     * list, empty object, empty string, null), otherwise returns true.
     */
    bool toBoolean(const Json& json) const;
    /**
     * @brief Evaluate the given function expression @a arguments.
     *
     * Evalutate ExpressionNodes on the current context to a JSON value and
     * evaluate ExpressionTypeNodes to their child ExpressionNode.
     * @param arguments List of FunctionExpressionNode arguments.
     * @return List of evaluated function arguments, suitable for passing to
     * built in functions.
     */
    FunctionArgumentList evaluateArguments(
            const FunctionExpressionArgumentList& arguments);
    /**
     * @brief Calculates the absolute value of the first item in the given list
     * of @a arguments. The first item must be a number JSON value.
     * @param arguments The list of the function's arguments.
     * @return Absolute value of the first item in @a arguments.
     * @throws InvalidFunctionArgumentType
     */
    Json abs(const FunctionArgumentList& arguments) const;
    /**
     * @brief Calculates the average value of the items in the first item of the
     * given @a arguments. The first item must be an JSON array and every item
     * in the array must be a number JSON value.
     * @param arguments The list of the function's arguments.
     * @return Average value of the items in the first item of the given
     * @a arguments
     * @throws InvalidFunctionArgumentType
     */
    Json avg(const FunctionArgumentList& arguments) const;
    /**
     * @brief Checks whether the first item in the given @a arguments contains
     * the second item. The first item should be either an array or string the
     * second item can be any JSON type.
     * @param arguments The list of the function's arguments.
     * @return Returns true if the first item contains the second, otherwise
     * returns false.
     * @throws InvalidFunctionArgumentType
     */
    Json contains(const FunctionArgumentList& arguments) const;
    /**
     * @brief Rounds up the first item of the given @a arguments to the next
     * highest integer value. The first item should be a JSON number.
     * @param arguments The list of the function's arguments.
     * @return Returns the next highest integer value of the first item.
     * @throws InvalidFunctionArgumentType
     */
    Json ceil(const FunctionArgumentList& arguments) const;
    /**
     * @brief Checks whether the first item of the given @a arguments ends with
     * the second item. The first and second item of @a arguments must be a
     * JSON string.
     * @param arguments The list of the function's arguments.
     * @return
     * @throws InvalidFunctionArgumentType
     */
    Json endsWith(const FunctionArgumentList& arguments) const;
    /**
     * @brief Rounds down the first item of the given @a arguments to the next
     * lowest integer value. The first item should be a JSON number.
     * @param arguments The list of the function's arguments.
     * @return Returns the next lowest integer value of the first item.
     * @throws InvalidFunctionArgumentType
     */
    Json floor(const FunctionArgumentList& arguments) const;
    /**
     * @brief Joins every item in the array provided as the second item of the
     * given @a arguments with the first item as a separator. The first item
     * must be a string and the second item must be an array of strings.
     * @param arguments The list of the function's arguments.
     * @return Returns the joined value.
     * @throws InvalidFunctionArgumentType
     */
    Json join(const FunctionArgumentList& arguments) const;
    /**
     * @brief Extracts the keys from the object provided as the first item of
     * the given @a arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns the array of keys.
     * @throws InvalidFunctionArgumentType
     */
    Json keys(const FunctionArgumentList& arguments) const;
    /**
     * @brief Returns the length of the first item in the given @a arguments.
     * The first item must be either an array a string or an object.
     * @param arguments The list of the function's arguments.
     * @return Returns the length.
     * @throws InvalidFunctionArgumentType
     */
    Json length(const FunctionArgumentList& arguments) const;
    /**
     * @brief Applies the expression provided as the first item in the given
     * @a arguments to every item in the array provided as the second item in
     * @a arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns the array of results.
     * @throws InvalidFunctionArgumentType
     */
    Json map(FunctionArgumentList& arguments);
    /**
     * @brief Finds the largest item in the array provided as the first item
     * in the @a arguments, it must either be an array of numbers or an array
     * of strings.
     * @param arguments The list of the function's arguments.
     * @return Returns the largest item.
     * @throws InvalidFunctionArgumentType
     */
    Json max(const FunctionArgumentList& arguments) const;
    /**
     * @brief Finds the largest item in the array provided as the first item
     * in the @a arguments, which must either be an array of numbers or an array
     * of strings, using the expression provided as the second item in @a
     * arguments as a key for comparison.
     * @param arguments The list of the function's arguments.
     * @return Returns the largest item.
     * @throws InvalidFunctionArgumentType
     */
    Json maxBy(FunctionArgumentList& arguments);
    /**
     * @brief Accepts zero or more objects in the given @a arguments, and
     * returns a single object with subsequent objects merged. Each subsequent
     * object’s key/value pairs are added to the preceding object.
     * @param arguments The list of the function's arguments.
     * @return Returns the merged object.
     * @throws InvalidFunctionArgumentType
     */
    Json merge(FunctionArgumentList& arguments) const;
    /**
     * @brief Finds the item with the lowest value in the array provided as the
     * first item in the @a arguments, it must either be an array of numbers or
     * an array of strings.
     * @param arguments The list of the function's arguments.
     * @return Returns the item with the lowest value.
     * @throws InvalidFunctionArgumentType
     */
    Json min(const FunctionArgumentList& arguments) const;
    /**
     * @brief Finds the item with the lowest value in the array provided as the
     * first item in the @a arguments, which must either be an array of numbers
     * or an array of strings, using the expression provided as the second item
     * in @a arguments as a key for comparison.
     * @param arguments The list of the function's arguments.
     * @return Returns the item with the lowest value.
     * @throws InvalidFunctionArgumentType
     */
    Json minBy(FunctionArgumentList& arguments);
    /**
     * @brief Accepts one or more items in @a arguments, and will evaluate them
     * in order until a non null argument is encounted.
     * @param arguments The list of the function's arguments.
     * @return Returns the first argument that does not resolve to null.
     * @throws InvalidFunctionArgumentType
     */
    Json notNull(FunctionArgumentList& arguments);
    /**
     * @brief Reverses the order of the first item in @a arguments. It must
     * either be an array or a string.
     * @param arguments The list of the function's arguments.
     * @return Returns the reversed item.
     * @throws InvalidFunctionArgumentType
     */
    Json reverse(FunctionArgumentList& arguments) const;
    /**
     * @brief Sorts the first item in the given @a arguments, which must either
     * be an array of numbers or an array of strings.
     * @param arguments The list of the function's arguments.
     * @return Returns the sorted array.
     * @throws InvalidFunctionArgumentType
     */
    Json sort(FunctionArgumentList& arguments) const;
    /**
     * @brief Sorts the first item in the given @a arguments, which must either
     * be an array of numbers or an array of strings. It uses the expression
     * provided as the second item in @a arguments as the sort key.
     * @param arguments The list of the function's arguments.
     * @return Returns the sorted array.
     * @throws InvalidFunctionArgumentType
     */
    Json sortBy(FunctionArgumentList& arguments);
    /**
     * @brief Checks wheather the string provided as the first item in @a
     * arguments starts with the string provided as the second item in @a
     * arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns true if the first item starts with the second item,
     * otherwise it returns false.
     * @throws InvalidFunctionArgumentType
     */
    Json startsWith(const FunctionArgumentList& arguments) const;
    /**
     * @brief Calculates the sum of the numbers in the array provided as the
     * first item of @a arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns the sum.
     * @throws InvalidFunctionArgumentType
     */
    Json sum(const FunctionArgumentList& arguments) const;
    /**
     * @brief Converts the first item of the given @a arguments to a one element
     * array if it's not already an array.
     * @param arguments The list of the function's arguments.
     * @return Returns the resulting array.
     * @throws InvalidFunctionArgumentType
     */
    Json toArray(FunctionArgumentList& arguments) const;
    /**
     * @brief Returns the JSON encoded value of the first item in the given
     * @a arguments as a string if it's not already a string.
     * @param arguments The list of the function's arguments.
     * @return Returns the string representation.
     * @throws InvalidFunctionArgumentType
     */
    Json toString(FunctionArgumentList& arguments) const;
    /**
     * @brief Converts the string provided as the first item in the given
     * @a arguments to a number. If it's already a number then the original
     * value is returned, all other JSON types are converted to null.
     * @param arguments The list of the function's arguments.
     * @return Returns the numeric representation.
     * @throws InvalidFunctionArgumentType
     */
    Json toNumber(FunctionArgumentList& arguments) const;
    /**
     * @brief Returns the type of the JSON value provided as the first item in
     * @a arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns the string representation of the type.
     * @throws InvalidFunctionArgumentType
     */
    Json type(const FunctionArgumentList& arguments) const;
    /**
     * @brief Extracts the values from the object provided as the first item of
     * the given @a arguments.
     * @param arguments The list of the function's arguments.
     * @return Returns the array of values.
     * @throws InvalidFunctionArgumentType
     */
    Json values(FunctionArgumentList& arguments) const;
    /**
     * @brief Finds the largest item in the array provided as the first item
     * in the @a arguments, it must either be an array of numbers or an array
     * of strings.
     * @param arguments The list of the function's arguments.
     * @param comparator The comparator function used for comparing JSON values.
     * @return Returns the largest item.
     * @throws InvalidFunctionArgumentType
     */
    Json maxElement(const FunctionArgumentList& arguments,
                    const JsonComparator& comparator = std::less<Json>{}) const;
    /**
     * @brief Finds the largest item in the array provided as the first item
     * in the @a arguments, which must either be an array of numbers or an array
     * of strings, using the expression provided as the second item in @a
     * arguments as a key for comparison.
     * @param arguments The list of the function's arguments.
     * @param comparator The comparator function used for comparing JSON values.
     * @return Returns the largest item.
     * @throws InvalidFunctionArgumentType
     */
    Json maxElementBy(FunctionArgumentList& arguments,
                      const JsonComparator& comparator = std::less<Json>{});
};
}} // namespace jmespath::interpreter
#endif // EXPRESSIONEVALUATOR_H
