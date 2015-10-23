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

std::string KLFunction::getNotation( std::string const &thisType ) const
{
  // [pzion 20151022] I have no idea what this accomplishes...
  if(m_returnType.length() == 0 && !isMethod())
    return getKLCode();

  FabricServices::ASTWrapper::KLMethod const *meth;
  if ( isMethod() )
    meth = static_cast<FabricServices::ASTWrapper::KLMethod const *>( this );
  else
    meth = 0;

  uint32_t typeGroupCount = 0;
  std::map<std::string, uint32_t> typeNameToTypeGroupIndex;

  if ( !m_returnType.empty() )
  {
    std::map<std::string, uint32_t>::iterator it =
      typeNameToTypeGroupIndex.find( m_returnType );
    if ( it == typeNameToTypeGroupIndex.end() )
      typeNameToTypeGroupIndex.insert(
        std::pair<std::string, uint32_t>( m_returnType, typeGroupCount++ )
        );
  }

  if ( meth && !meth->isConstructor() )
  {
    std::map<std::string, uint32_t>::iterator it =
      typeNameToTypeGroupIndex.find( thisType );
    if ( it == typeNameToTypeGroupIndex.end() )
      typeNameToTypeGroupIndex.insert(
        std::pair<std::string, uint32_t>( thisType, typeGroupCount++ )
        );
  }

  for ( uint32_t i = 0; i < m_params.size(); ++i )
  {
    const KLParameter * param = m_params[i];
    std::string paramType = param->getTypeNoArray();
    std::map<std::string, uint32_t>::iterator it =
      typeNameToTypeGroupIndex.find( paramType );
    if ( it == typeNameToTypeGroupIndex.end() )
      typeNameToTypeGroupIndex.insert(
        std::pair<std::string, uint32_t>( paramType, typeGroupCount++ )
        );
  }

  std::ostringstream notation;

  if ( !m_returnType.empty() )
  {
    notation << "$TYPE";
    notation << typeNameToTypeGroupIndex[m_returnType];
    notation << "$ ";
  }

  if ( meth && !meth->isConstructor() )
  {
    notation << thisType;
    notation << ".";
  }

  notation << getName();

  if ( meth && !meth->isConstructor() )
    notation << getSuffix();

  notation << "(";

  for ( uint32_t i = 0; i < m_params.size(); ++i )
  {
    const KLParameter * param = m_params[i];

    if ( i > 0 )
      notation << ", ";
    
    notation << param->getUsage();
    notation << " $TYPE";
    notation << typeNameToTypeGroupIndex[param->getTypeNoArray()];
    notation << "$";
    notation << param->getTypeArraySuffix();
  }

  notation << ")";

  return notation.str();
}

std::string KLFunction::getLabel() const
{
  if(m_label.length() == 0)
    m_label = getKLCode(false, false, false, true);
  return m_label;
}

