// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLTypeOp.h"

#include <FTL/StrToLower.h>

using namespace FabricServices::ASTWrapper;

KLTypeOp::KLTypeOp(const KLFile* klFile, JSONData data)
: KLFunction(klFile, data)
{
  m_isUnary = false;

  std::string type = getStringDictValue("type");
  if(type == "ComparisonOpImpl")
  {
    std::string binOpType = getStringDictValue("binOpType");
    if(binOpType == "eq")
    {
      m_op = OpType_Equal;
    }
    else if(binOpType == "ne")
    {
      m_op = OpType_NotEqual;
    }
    else if(binOpType == "lt")
    {
      m_op = OpType_LessThan;
    }
    else if(binOpType == "gt")
    {
      m_op = OpType_GreaterThan;
    }
    else if(binOpType == "le")
    {
      m_op = OpType_LessEquals;
    }
    else if(binOpType == "ge")
    {
      m_op = OpType_GreaterEquals;
    }
    else
    {
      std::string message = "ComparisonOpImpl contains unsupport binOpType '"+binOpType+"'.";
      throw(FabricCore::Exception(message.c_str()));
    }

    m_lhs = getDictValue("lhs")->getDictValue("typeUserName")->getStringData();
    m_rhs = getDictValue("rhs")->getDictValue("typeUserName")->getStringData();
  }
  else if(type == "AssignOpImpl")
  {
    m_op = OpType_Assign;
    m_lhs = getStringDictValue("thisType");
    m_rhs = getDictValue("rhs")->getDictValue("typeUserName")->getStringData();
  }
  else if(type == "BinOpImpl")
  {
    std::string binOpType = getStringDictValue("binOpType");
    FTL::StrToLower( binOpType );
    if(binOpType == "add")
    {
      m_op = OpType_Add;
    }
    else if(binOpType == "sub")
    {
      m_op = OpType_Sub;
    }
    else if(binOpType == "mul")
    {
      m_op = OpType_Mul;
    }
    else if(binOpType == "div")
    {
      m_op = OpType_Div;
    }
    else
    {
      std::string message = "BinOpImpl contains unsupport binOpType '"+binOpType+"'.";
      throw(FabricCore::Exception(message.c_str()));
    }

    m_lhs = getDictValue("lhs")->getDictValue("typeUserName")->getStringData();
    m_rhs = getDictValue("rhs")->getDictValue("typeUserName")->getStringData();
  }
  else if(type == "ASTUniOpDecl")
  {
    std::string uniOpType = getStringDictValue("uniOpType");
    FTL::StrToLower(uniOpType);
    if(uniOpType == "neg")
    {
      m_op = OpType_Neg;
    }
    else
    {
      std::string message = "ASTUniOpDecl contains unsupport uniOpType '"+uniOpType+"'.";
      throw(FabricCore::Exception(message.c_str()));
    }
    m_rhs = getStringDictValue("thisType");
    m_lhs = m_rhs;
    m_isUnary = true;
  }
  else
  {
    std::string message = "KLTypeOp contains unsupport type '"+type+"'.";
    throw(FabricCore::Exception(message.c_str()));
  }
}

KLTypeOp::~KLTypeOp()
{
}

KLDeclType KLTypeOp::getDeclType() const
{
  return KLDeclType_TypeOp;
}

bool KLTypeOp::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_TypeOp)
    return true;
  return KLFunction::isOfDeclType(type);
}

const std::string & KLTypeOp::getName() const
{
  if(m_name.length() == 0)
  {
    switch(m_op)
    {
      case(OpType_Equal):
      {
        m_name = "==";
        break;
      }
      case(OpType_NotEqual):
      {
        m_name = "!=";
        break;
      }
      case(OpType_LessThan):
      {
        m_name = "<";
        break;
      }
      case(OpType_GreaterThan):
      {
        m_name = ">";
        break;
      }
      case(OpType_LessEquals):
      {
        m_name = "<=";
        break;
      }
      case(OpType_GreaterEquals):
      {
        m_name = ">=";
        break;
      }
      case(OpType_Assign):
      {
        m_name = "=";
        break;
      }
      case(OpType_Add):
      {
        m_name = "+";
        break;
      }
      case(OpType_Sub):
      {
        m_name = "-";
        break;
      }
      case(OpType_Mul):
      {
        m_name = "*";
        break;
      }
      case(OpType_Div):
      {
        m_name = "/";
        break;
      }
      case(OpType_Neg):
      {
        m_name = "-";
        break;
      }
      case(OpType_NumElements): break;
    }
  }
  return m_name;
}

const std::string & KLTypeOp::getReturnType() const
{
  if(m_returnType.length() == 0)
  {
    switch(m_op)
    {
      case(OpType_Equal):
      case(OpType_NotEqual):
      case(OpType_LessThan):
      case(OpType_GreaterThan):
      case(OpType_LessEquals):
      case(OpType_GreaterEquals):
      {
        m_returnType = "Boolean";
        break;
      }
      case(OpType_Assign):
      case(OpType_Add):
      case(OpType_Sub):
      case(OpType_Mul):
      case(OpType_Div):
      {
        m_returnType = getLhs();
        break;
      }
      case(OpType_Neg):
      {
        break;
      }
      case(OpType_NumElements): break;
    }
  }
  return m_returnType;
}

uint32_t KLTypeOp::getParameterCount() const
{
  return 0;
}

const KLParameter * KLTypeOp::getParameter(uint32_t index) const
{
  return NULL;
}

bool KLTypeOp::isMethod() const
{
  return false;
}

std::string KLTypeOp::getPrefix() const
{
  return "";
} 

std::string KLTypeOp::getSuffix() const
{
  return "";
} 

const char * KLTypeOp::getKLType() const
{
  return "function";
}

std::string KLTypeOp::getKLCode(bool includeReturnType, bool includeKeyWord, bool includePrefix, bool includeName) const
{
  std::string code;

  if(includeKeyWord)
  {
    code += getKLType();
    code += " ";
  }

  if(includeReturnType)
  {
    std::string returnType = getReturnType();
    if(returnType.length() > 0)
      code += returnType + " ";
  }

  if(includeName)
  {
    code += getName() + " ";
  }

  switch(m_op)
  {
    case(OpType_Equal):
    case(OpType_NotEqual):
    case(OpType_LessThan):
    case(OpType_GreaterThan):
    case(OpType_LessEquals):
    case(OpType_GreaterEquals):
    {
      code += "( "+getLhs()+" a, "+getRhs()+" b )";
      break;
    }
    case(OpType_Assign):
    {
      code += "( "+getRhs()+" other )";
      break;
    }
    case(OpType_Add):
    case(OpType_Sub):
    case(OpType_Mul):
    case(OpType_Div):
    {
      code += "( "+getLhs()+" a, "+getRhs()+" b )";
      break;
    }
    case(OpType_Neg):
    {
      code += getLhs()+"()";
      break;
    }
    case(OpType_NumElements): break;
  }

  return code;
}

std::string KLTypeOp::getLabel() const
{
  if(m_label.length() == 0)
    m_label = getKLCode(true, false, true, true);
  return m_label;
}

bool KLTypeOp::isUnary() const
{
  return m_isUnary;
}

KLTypeOp::OpType KLTypeOp::getOpType() const
{
  return m_op;
}

std::string KLTypeOp::getLhs() const
{
  return m_lhs;
}

std::string KLTypeOp::getRhs() const
{
  return m_rhs;
}

