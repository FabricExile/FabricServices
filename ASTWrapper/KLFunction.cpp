// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "KLFunction.h"
#include "KLMethod.h"
#include "KLExtension.h"

#include <vector>
#include <string>
#include <sstream>

using namespace FabricServices::ASTWrapper;

KLFunction::KLFunction(const KLFile* klFile, JSONData data)
: KLStmt(klFile, data)
{
  const char * name = getStringDictValue("name");
  if(name)
    m_name = name;

  JSONData flagsVal = getDictValue("flags");
  if(flagsVal)
    m_flags = flagsVal->getSInt32();
  else
    m_flags = 0;

  const char * access = getStringDictValue("access");
  if(access)
    m_access = access;

  const char * returnType = getStringDictValue("returnType");
  if(returnType)
    m_returnType = returnType;

  const char * symbolName = getStringDictValue("symbolName");
  if(symbolName)
    m_symbolName = symbolName;

  JSONData params = getArrayDictValue("params");
  if(params)
  {
    for(uint32_t i=0;i<params->getArraySize();i++)
    {
      KLParameter * param = new KLParameter(klFile, params->getArrayElement(i));
      m_params.push_back(param);
    }
  }

  JSONData body = getDictValue("body");
  if(body)
    m_body = (KLCompoundStmt *)constructChild(body);
  else
    m_body = NULL;

}

KLFunction::~KLFunction()
{
  for(uint32_t i=0;i<m_params.size();i++)
  {
    delete(m_params[i]);
  }
}

KLDeclType KLFunction::getDeclType() const
{
  return KLDeclType_Function;
}

bool KLFunction::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Function)
    return true;
  return KLStmt::isOfDeclType(type);
}

const std::string & KLFunction::getName() const
{
  return m_name;
}

const std::string & KLFunction::getReturnType() const
{
  return m_returnType;
}

const std::string & KLFunction::getSymbolName() const
{
  return m_symbolName;
}

uint32_t KLFunction::getParameterCount() const
{
  return m_params.size();
}

const KLParameter * KLFunction::getParameter(uint32_t index) const
{
  return m_params[index];
}

const KLCompoundStmt * KLFunction::getBody() const
{
  return m_body;
}

bool KLFunction::hasUniqueName() const
{
  std::vector<const KLFunction*> funcs = getExtension()->getFunctions();

  for(size_t i=0;i<funcs.size();i++)
  {
    if(funcs[i] == this)
      continue;
    if(funcs[i]->getName() == getName())
      return false;
  }

  return true;
}

bool KLFunction::isMethod() const
{
  return false;
}

bool KLFunction::isPrivate() const
{
  return m_access == "private";
}

std::string KLFunction::getPrefix() const
{
  return "";
}

std::string KLFunction::getSuffix() const
{
  return "";
}

const char * KLFunction::getKLType() const
{
  return "function";
}

std::string KLFunction::getKLCode(bool includeReturnType, bool includeKeyWord, bool includePrefix, bool includeName) const
{
  std::string code;

  if(includeKeyWord)
  {
    code += getKLType();
    code += " ";
  }

  if(m_returnType.length() > 0 && includeReturnType)
  {
    code += m_returnType;
    code += " ";
  }

  if(includePrefix)
    code += getPrefix();

  if(includeName)
    code += getName();

  bool skipSuffix = false;

  if(isMethod())
  {
    const KLMethod * method = (const KLMethod *)this;
    if("~"+method->getThisType() == getName())
      skipSuffix = true;
  }

  if(!skipSuffix)
    code += getSuffix();

  code += "(";

  if(m_params.size() > 0)
  {
    code += " ";

    for(uint32_t i=0;i<m_params.size();i++)
    {
      const KLParameter * p = m_params[i];
      if(i > 0)
        code += ", ";

      code += p->getUsage();
      code += " ";
      code += p->getTypeNoArray();
      code += " ";
      code += p->getName();
      code += p->getTypeArraySuffix();
    }


    code += " ";
  }

  code += ")";

  return code;
}

std::string KLFunction::getNotation(
  FTL::StrRef thisTypeOverride,
  bool isPolyThis,
  bool isPolyParams,
  bool includeParams
  ) const
{
  KLMethod const *meth;
  if ( isMethod() )
    meth = static_cast<KLMethod const *>( this );
  else
    meth = 0;

  if ( m_returnType.length() == 0 && !meth )
    return getKLCode();

  std::string thisTypeToReplace;
  if ( isPolyThis )
  {
    if ( meth )
      thisTypeToReplace = meth->getThisType();
    else
      thisTypeToReplace = m_returnType;
  }

  std::ostringstream ss;

  if ( isPolyThis )
  {
    if ( m_returnType.length() > 0 )
    {
      if ( thisTypeToReplace == m_returnType )
        ss << "$TYPE$";
      else
        ss << m_returnType;
      ss << " ";
    }
  }

  if ( meth && !meth->isConstructor() )
  {
    if ( isPolyThis )
      ss << "$TYPE$";
    else
      ss << thisTypeOverride;
    ss << ".";
  }

  std::string dfgPresetTitle =
    getComments()->getSingleQualifier("dfgPresetTitle");
  if ( !dfgPresetTitle.empty() )
    ss << dfgPresetTitle;
  else
    ss << getName();

  if ( includeParams )
  {
    if ( !meth || !meth->isConstructor() )
      ss << getSuffix();

    ss << '(';

    for ( uint32_t i = 0; i < m_params.size(); ++i )
    {
      const KLParameter * p = m_params[i];
      if(i > 0)
        ss << ", ";

      ss << p->getUsage();
      ss << ' ';
      if ( p->getType() == thisTypeToReplace )
        ss << "$TYPE$";
      else if ( isPolyParams )
      {
        ss << "$TYPE_PARAM_";
        ss << i + 1;
        ss << '$';
      }
      else
        ss << p->getType();
    }

    ss << ')';
  }

  return ss.str();
}

std::string KLFunction::getLabel() const
{
  if(m_label.length() == 0)
    m_label = getKLCode(false, false, false, true);
  return m_label;
}

