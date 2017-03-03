// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLMember.h"
#include "KLType.h"
#include "KLASTManager.h"

#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;

KLMember::KLMember(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data)
: KLCommented(klFile, nameSpace, data)
{
  m_name = getDictValue("memberDecls")->getArrayElement(0)->getDictValue("name")->getStringData();
  m_type = getStringDictValue("baseType");
  m_type += getDictValue("memberDecls")->getArrayElement(0)->getDictValue("arrayModifier")->getStringData();
}

KLMember::~KLMember()
{
}

KLDeclType KLMember::getDeclType() const
{
  return KLDeclType_Member;
}

bool KLMember::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Member)
    return true;
  return KLCommented::isOfDeclType(type);
}

const std::string & KLMember::getName() const
{
  return m_name;
}

std::string KLMember::getType(bool includeNameSpace) const
{
  if(includeNameSpace)
  {
    const KLType * klType = getASTManager()->getKLTypeByName(getTypeNoArray(false).c_str(), this);
    if(klType)
      return klType->getNameSpacePrefix() + m_type;
  }
  return m_type;
}

std::string KLMember::getTypeNoArray(bool includeNameSpace) const
{
  std::vector<std::string> parts;
  FTL::StrSplit<'['>( getType(includeNameSpace), parts );
  std::string type = parts[0];
  if(type.substr(type.length()-2, 2) == "<>")
    type = type.substr(0, type.length()-2);
  return type;
}

std::string KLMember::getTypeArraySuffix() const
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
