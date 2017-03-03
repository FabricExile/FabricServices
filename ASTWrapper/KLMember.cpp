// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLMember.h"

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

const std::string & KLMember::getType() const
{
  return m_type;
}
