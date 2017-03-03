// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLParameter.h"
#include "KLType.h"
#include "KLASTManager.h"

#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;

KLParameter::KLParameter(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data)
: KLDecl(klFile, nameSpace, data)
{
  m_usage = getStringDictValue("usage");
  m_name = getStringDictValue("name");
  m_type = getStringDictValue("typeUserName");
}

KLParameter::~KLParameter()
{
}

KLDeclType KLParameter::getDeclType() const
{
  return KLDeclType_Parameter;
}

bool KLParameter::isOfDeclType(KLDeclType type) const
{
  return type == KLDeclType_Parameter;
}

const std::string & KLParameter::getUsage() const
{
  return m_usage;
}

const std::string & KLParameter::getName() const
{
  return m_name;
}

std::string KLParameter::getType(bool includeNameSpace) const
{
  if(includeNameSpace)
  {
    const KLType * klType = getASTManager()->getKLTypeByName(getTypeNoArray(false).c_str(), this);
    if(klType)
      return klType->getNameSpacePrefix() + m_type;
  }
  return m_type;
}

std::string KLParameter::getTypeNoArray(bool includeNameSpace) const
{
  std::vector<std::string> parts;
  FTL::StrSplit<'['>( getType(includeNameSpace), parts );
  std::string type = parts[0];
  if(type.substr(type.length()-2, 2) == "<>")
    type = type.substr(0, type.length()-2);
  return type;
}

std::string KLParameter::getTypeArraySuffix() const
{
  if(m_type.substr(m_type.length()-2, 2) == "<>")
    return "<>";
  if(m_type.substr(m_type.length()-1, 1) == "]")
  {
    std::vector<std::string> parts;
    FTL::StrSplit<'['>( m_type, parts );
    return "[" + parts[1];
  }
  return "";
}

