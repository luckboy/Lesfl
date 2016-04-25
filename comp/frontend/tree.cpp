/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <lesfl/frontend/tree.hpp>

namespace lesfl
{
  namespace frontend
  {
    //
    // A Positional class.
    //

    Positional::~Positional() {}

    //
    // An Identifier class.
    //

    Identifier::~Identifier() {}

    //
    // An AbsoluteIdentifier class.
    //

    AbsoluteIdentifier::~AbsoluteIdentifier() {}

    //
    // A RelativeIdentifier class.
    //

    RelativeIdentifier::~RelativeIdentifier() {}

    //
    // A Tree class.
    //

    Tree::~Tree() {}
    
    //
    // A Definition class.
    //

    Definition::~Definition() {}

    //
    // An Import class.
    //

    Import::~Import() {}

    //
    // A ModuleDefinition class.
    //

    ModuleDefinition::~ModuleDefinition() {}

    //
    // A VariableDefinition class.
    //

    VariableDefinition::~VariableDefinition() {}

    //
    // A FunctionDefinition class.
    //

    FunctionDefinition::~FunctionDefinition() {}

    //
    // A FunctionInstanceDefinition class.
    //

    FunctionInstanceDefinition::~FunctionInstanceDefinition() {}

    //
    // A TypeVariableDefinition class.
    //

    TypeVariableDefinition::~TypeVariableDefinition() {}

    //
    // A TypeFunctionDefinition class.
    //

    TypeFunctionDefinition::~TypeFunctionDefinition() {}

    //
    // A TypeFunctionInstanceDefinition class.
    //

    TypeFunctionInstanceDefinition::~TypeFunctionInstanceDefinition() {}

    //
    // A Variable class.
    //

    Variable::~Variable() {}

    //
    // An UserDefinedVariable class.
    //

    UserDefinedVariable::~UserDefinedVariable() {}

    //
    // An ExternalVariable class.
    //

    ExternalVariable::~ExternalVariable() {}

    //
    // A FunctionVariable class.
    //

    FunctionVariable::~FunctionVariable() {}

    //
    // A Function class.
    //

    Function::~Function() {}

    //
    // A DefinedFunction class.
    //

    DefinedFunction::~DefinedFunction() {}

    //
    // An UserDefinedFunction class.
    //

    UserDefinedFunction::~UserDefinedFunction() {}

    //
    // An ExternalFunction class.
    //

    ExternalFunction::~ExternalFunction() {}

    //
    // A NativeFunction class.
    //

    NativeFunction::~NativeFunction() {}

    //
    // A FunctionInstance class.
    //

    FunctionInstance::~FunctionInstance() {}

    //
    // An Argument class.
    //

    Argument::~Argument() {}

    //
    // An Annotation class.
    //

    Annotation::~Annotation() {}

    //
    // An Expression class.
    //

    Expression::~Expression() {}

    //
    // A Literal class.
    //

    Literal::~Literal() {}

    //
    // A Collection class.
    //

    Collection::~Collection() {}

    //
    // A List class.
    //

    List::~List() {}

    //
    // An Array class.
    //

    Array::~Array() {}

    //
    // A NonUniqueArray class.
    //

    NonUniqueArray::~NonUniqueArray() {}

    //
    // An UniqueArray class.
    //

    UniqueArray::~UniqueArray() {}

    //
    // A Tuple class.
    //

    Tuple::~Tuple() {}

    //
    // A NonUniqueTuple class.
    //

    NonUniqueTuple::~NonUniqueTuple() {}

    //
    // An UniqueTuple class.
    //

    UniqueTuple::~UniqueTuple() {}

    //
    // A VariableExpression class.
    //

    VariableExpression::~VariableExpression() {}

    //
    // A NamedFieldConstructorApplication class.
    //

    NamedFieldConstructorApplication::~NamedFieldConstructorApplication() {}

    //
    // A NonUniqueNamedFieldConstructorApplication class.
    //

    NonUniqueNamedFieldConstructorApplication::~NonUniqueNamedFieldConstructorApplication() {}

    //
    // An UniqueNamedFieldConstructorApplication class.
    //

    UniqueNamedFieldConstructorApplication::~UniqueNamedFieldConstructorApplication() {}

    //
    // An Application class.
    //

    Application::~Application() {}

    //
    // A NonUniqueApplication class.
    //

    NonUniqueApplication::~NonUniqueApplication() {}

    //
    // An UniqueApplication class.
    //

    UniqueApplication::~UniqueApplication() {}

    //
    // A BuiltinApplication class.
    //

    BuiltinApplication::~BuiltinApplication() {}

    //
    // A FieldOperator class.
    //

    FieldOperator::~FieldOperator() {}

    //
    // A Field class.
    //

    Field::~Field() {}

    //
    // An UniqueField class.
    //

    UniqueField::~UniqueField() {}

    //
    // A SetUniqueField class.
    //

    SetUniqueField::~SetUniqueField() {}

    //
    // A TypedExpression class.
    //

    TypedExpression::~TypedExpression() {}

    //
    // A Let class.
    //

    Let::~Let() {}

    //
    // A Match class.
    //

    Match::~Match() {}

    //
    // A Throw class.
    //

    Throw::~Throw() {}

    //
    // An ExpressionNamedFieldPair class.
    //

    ExpressionNamedFieldPair::~ExpressionNamedFieldPair() {}

    //
    // A Bind class.
    //

    Bind::~Bind() {}

    //
    // A VariableBind class.
    //

    VariableBind::~VariableBind() {}

    //
    // A TupleBind class.
    //

    TupleBind::~TupleBind() {}

    //
    // A TupleBindVariable class.
    //

    TupleBindVariable::~TupleBindVariable() {}

    //
    // A Case class.
    //

    Case::~Case() {}

    //
    // A Pattern class.
    //

    Pattern::~Pattern() {}

    //
    // A ConstructorPattern class.
    //

    ConstructorPattern::~ConstructorPattern() {}

    //
    // An UnnamedFieldConstructorPattern class.
    //

    UnnamedFieldConstructorPattern::~UnnamedFieldConstructorPattern() {}

    //
    // A NamedFieldConstructorPattern class.
    //

    NamedFieldConstructorPattern::~NamedFieldConstructorPattern() {}

    //
    // A CollectionPattern class.
    //

    CollectionPattern::~CollectionPattern() {}

    //
    // A ListPattern class.
    //

    ListPattern::~ListPattern() {}

    //
    // An ArrayPattern class.
    //

    ArrayPattern::~ArrayPattern() {}

    //
    // A NonUniqueArrayPattern class.
    //

    NonUniqueArrayPattern::~NonUniqueArrayPattern() {}

    //
    // An UniqueArrayPattern class.
    //

    UniqueArrayPattern::~UniqueArrayPattern() {}

    //
    // A TuplePattern class.
    //

    TuplePattern::~TuplePattern() {}

    //
    // A NonUniqueTuplePattern class.
    //

    NonUniqueTuplePattern::~NonUniqueTuplePattern() {}

    //
    // An UniqueTuplePattern class.
    //

    UniqueTuplePattern::~UniqueTuplePattern() {}

    //
    // A LiteralPattern class.
    //

    LiteralPattern::~LiteralPattern() {}

    //
    // A VariablePattern class.
    //

    VariablePattern::~VariablePattern() {}

    //
    // A PatternNamedFieldPair class.
    //

    PatternNamedFieldPair::~PatternNamedFieldPair() {}

    //
    // A LiteralValue class.
    //

    LiteralValue::~LiteralValue() {}

    //
    // A NonUniqueLiteralValue class.
    //

    NonUniqueLiteralValue::~NonUniqueLiteralValue() {}

    //
    // A CharValue class.
    //

    CharValue::~CharValue() {}

    //
    // An IntValue class.
    //

    IntValue::~IntValue() {}

    //
    // A FloatValue class.
    //

    FloatValue::~FloatValue() {}

    //
    // A StringValue class.
    //

    StringValue::~StringValue() {}

    //
    // A LambdaValue class.
    //

    LambdaValue::~LambdaValue() {}

    //
    // A NonUniqueLambdaValue class.
    //

    NonUniqueLambdaValue::~NonUniqueLambdaValue() {}

    //
    // An UniqueLambdaValue class.
    //

    UniqueLambdaValue::~UniqueLambdaValue() {}

    //
    // A Value class.
    //

    Value::~Value() {}

    //
    // A ValueNamedFieldPair class.
    //

    ValueNamedFieldPair::~ValueNamedFieldPair() {}

    //
    // A TypeVariable class.
    //

    TypeVariable::~TypeVariable() {}

    //
    // A TypeFunction class.
    //

    TypeFunction::~TypeFunction() {}

    //
    // A TypeFunctionInstance class.
    //

    TypeFunctionInstance::~TypeFunctionInstance() {}

    //
    // A Datatype class.
    //

    Datatype::~Datatype() {}

    //
    // A NonUniqueDatatype class.
    //

    NonUniqueDatatype::~NonUniqueDatatype() {}

    //
    // An UniqueDatatype class.
    //

    UniqueDatatype::~UniqueDatatype() {}

    //
    // A Constructor class.
    //

    Constructor::~Constructor() {}

    //
    // An UnnamedFieldConstructor class.
    //

    UnnamedFieldConstructor::~UnnamedFieldConstructor() {}

    //
    // A NamedFieldConstructor class.
    //

    NamedFieldConstructor::~NamedFieldConstructor() {}

    //
    // A TypeArgument class.
    //

    TypeArgument::~TypeArgument() {}

    //
    // A TypeParameter class.
    //

    TypeParameter::~TypeParameter() {}

    //
    // A TypeNamedFieldPair class.
    //

    TypeNamedFieldPair::~TypeNamedFieldPair() {}

    //
    // A TypeExpression class.
    //

    TypeExpression::~TypeExpression() {}
  }
}
