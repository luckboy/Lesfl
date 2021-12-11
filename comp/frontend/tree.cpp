/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <lesfl/frontend/tree.hpp>
#include "frontend/ident.hpp"

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    //
    // A Positional class.
    //

    Positional::~Positional() {}

    //
    // An Inlinable class.
    //

    Inlinable::~Inlinable() {}

    //
    // An Accessible class.
    //

    Accessible::~Accessible() {}

    //
    // An Identifiable class.
    //

    Identifiable::~Identifiable() {}
    
    string Identifiable::to_ident_string() const
    { return priv::ident_to_string(_M_ident); }

    //
    // An IdentifiableAndIndexable class.
    //

    IdentifiableAndIndexable::~IdentifiableAndIndexable() {}

    //
    // An Instance class.
    //
    
    Instance::~Instance() {}

    //
    // A VariableInfo class.
    //

    VariableInfo::~VariableInfo() {}

    //
    // A TypeVariableInfo class.
    //

    TypeVariableInfo::~TypeVariableInfo() {}

    //
    // A TypeFunctionInfo class.
    //

    TypeFunctionInfo::~TypeFunctionInfo() {}

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
    // A VariableInstanceDefinition class.
    //

    VariableInstanceDefinition::~VariableInstanceDefinition() {}

    //
    // A TypeVariableDefinition class.
    //

    TypeVariableDefinition::~TypeVariableDefinition() {}

    //
    // A FunctionDefinition class.
    //

    FunctionDefinition::~FunctionDefinition() {}

    //
    // A FunctionInstanceDefinition class.
    //

    FunctionInstanceDefinition::~FunctionInstanceDefinition() {}

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
    // An OriginalVariable class.
    //

    OriginalVariable::~OriginalVariable() {}

    //
    // An InstanceVariable class.
    //

    InstanceVariable::~InstanceVariable() {}

    //
    // A DefinableVariable class.
    //

    DefinableVariable::~DefinableVariable() {}

    //
    // An UserDefinedVariable class.
    //

    UserDefinedVariable::~UserDefinedVariable() {}

    //
    // An ExternalVariable class.
    //

    ExternalVariable::~ExternalVariable() {}

    //
    // An AliasVariable class.
    //

    AliasVariable::~AliasVariable() {}

    //
    // A FunctionVariable class.
    //

    FunctionVariable::~FunctionVariable() {}

    //
    // A ConstructorVariable class.
    //

    ConstructorVariable::~ConstructorVariable() {}

    //
    // A LibraryVariable class.
    //

    LibraryVariable::~LibraryVariable() {}

    //
    // A VariableInstance class.
    //

    VariableInstance::~VariableInstance() {}

    //
    // A Function class.
    //

    Function::~Function() {}

    //
    // An OriginalFunction class.
    //

    OriginalFunction::~OriginalFunction() {}

    //
    // An InstanceFunction class.
    //

    InstanceFunction::~InstanceFunction() {}

    //
    // A DefinableFunction class.
    //

    DefinableFunction::~DefinableFunction() {}

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

    string Annotation::to_string() const
    { return "@" + priv::ident_to_string(_M_ident); }

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
    // A NamedFieldOperator class.
    //

    NamedFieldOperator::~NamedFieldOperator() {}

    //
    // A NamedField class.
    //

    NamedField::~NamedField() {}

    //
    // An UniqueField class.
    //

    UniqueNamedField::~UniqueNamedField() {}

    //
    // A SetUniqueField class.
    //

    SetUniqueNamedField::~SetUniqueNamedField() {}

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
    // A Binding class.
    //

    Binding::~Binding() {}

    //
    // A VariableBinding class.
    //

    VariableBinding::~VariableBinding() {}

    //
    // A TupleBinding class.
    //

    TupleBinding::~TupleBinding() {}

    //
    // A TupleBindingVariable class.
    //

    TupleBindingVariable::~TupleBindingVariable() {}

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
    // A VariableConstructorPattern class.
    //

    VariableConstructorPattern::~VariableConstructorPattern() {}

    //
    // A FunctionConstructorPattern class.
    //

    FunctionConstructorPattern::~FunctionConstructorPattern() {}

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
    // An AsPattern class.
    //

    AsPattern::~AsPattern() {}

    //
    // A WildcardPattern class.
    //

    WildcardPattern::~WildcardPattern() {}

    //
    // A TypedPattern class.
    //

    TypedPattern::~TypedPattern() {}

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
    // A SimpleLiteralValue class.
    //

    SimpleLiteralValue::~SimpleLiteralValue() {}

    //
    // A CharValue class.
    //

    CharValue::~CharValue() {}

    //
    // A WideCharValue class.
    //

    WideCharValue::~WideCharValue() {}

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
    // A WideStringValue class.
    //

    WideStringValue::~WideStringValue() {}

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
    // A VariableLiteralValue class.
    //

    VariableLiteralValue::~VariableLiteralValue() {}

    //
    // A CollectionValue class.
    //

    CollectionValue::~CollectionValue() {}

    //
    // A ListValue class.
    //

    ListValue::~ListValue() {}

    //
    // An ArrayValue class.
    //

    ArrayValue::~ArrayValue() {}

    //
    // A TupleValue class.
    //

    TupleValue::~TupleValue() {}

    //
    // A ConstructorValue class.
    //

    ConstructorValue::~ConstructorValue() {}

    //
    // A VariableConstructorValue class.
    //

    VariableConstructorValue::~VariableConstructorValue() {}

    //
    // A FunctionConstructorValue class.
    //

    FunctionConstructorValue::~FunctionConstructorValue() {}

    //
    // An UnnamedFieldConstructorValue class.
    //

    UnnamedFieldConstructorValue::~UnnamedFieldConstructorValue() {}

    //
    // A NamedFieldConstructorValue class.
    //

    NamedFieldConstructorValue::~NamedFieldConstructorValue() {}

    //
    // A TypedValue class.
    //

    TypedValue::~TypedValue() {}

    //
    // A ValueNamedFieldPair class.
    //

    ValueNamedFieldPair::~ValueNamedFieldPair() {}

    //
    // A TypeVariable class.
    //

    TypeVariable::~TypeVariable() {}

    //
    // A DefinableTypeVariable class.
    //

    DefinableTypeVariable::~DefinableTypeVariable() {}

    //
    // A TypeSynonymVariable class.
    //

    TypeSynonymVariable::~TypeSynonymVariable() {}

    //
    // A DatatypeVariable class.
    //

    DatatypeVariable::~DatatypeVariable() {}

    //
    // A BuiltinTypeVariable class.
    //

    BuiltinTypeVariable::~BuiltinTypeVariable() {}

    //
    // A TypeFunction class.
    //

    TypeFunction::~TypeFunction() {}

    //
    // A DefinableTypeFunction class.
    //

    DefinableTypeFunction::~DefinableTypeFunction() {}

    //
    // A TypeSynonymFunction class.
    //

    TypeSynonymFunction::~TypeSynonymFunction() {}

    //
    // A DatatypeFunction class.
    //

    DatatypeFunction::~DatatypeFunction() {}

    //
    // A BuiltinTypeFunction class.
    //

    BuiltinTypeFunction::~BuiltinTypeFunction() {}

    //
    // A TypeFunctionInstance class.
    //

    TypeFunctionInstance::~TypeFunctionInstance() {}

    //
    // A TypeSynonymFunctionInstance class.
    //

    TypeSynonymFunctionInstance::~TypeSynonymFunctionInstance() {}

    //
    // A DatatypeFunctionInstance class.
    //

    DatatypeFunctionInstance::~DatatypeFunctionInstance() {}

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
    // A VariableConstructor class.
    //

    VariableConstructor::~VariableConstructor() {}

    //
    // A FunctionConstructor class.
    //

    FunctionConstructor::~FunctionConstructor() {}

    //
    // An UnnamedFieldConstructor class.
    //

    UnnamedFieldConstructor::~UnnamedFieldConstructor() {}

    std::size_t UnnamedFieldConstructor::field_count() { return _M_field_types->size(); }
    
    //
    // A NamedFieldConstructor class.
    //

    NamedFieldConstructor::~NamedFieldConstructor() {}

    std::size_t NamedFieldConstructor::field_count() { return _M_field_types->size(); }

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

    //
    // A TypeExpression class.
    //

    With::~With() {}

    //
    // A TypeVariableExpression class.
    //

    TypeVariableExpression::~TypeVariableExpression() {}

    //
    // A TypeParameterExpression class.
    //

    TypeParameterExpression::~TypeParameterExpression() {}

    //
    // A TupleType class.
    //

    TupleType::~TupleType() {}

    //
    // A NonUniqueTupleType class.
    //

    NonUniqueTupleType::~NonUniqueTupleType() {}

    //
    // An UniqueTupleType class.
    //

    UniqueTupleType::~UniqueTupleType() {}

    //
    // A FunctionType class.
    //

    FunctionType::~FunctionType() {}

    //
    // A NonUniqueFunctionType class.
    //

    NonUniqueFunctionType::~NonUniqueFunctionType() {}

    //
    // An UniqueFunctionType class.
    //

    UniqueFunctionType::~UniqueFunctionType() {}

    //
    // A TypeApplication class.
    //

    TypeApplication::~TypeApplication() {}
  }
}
