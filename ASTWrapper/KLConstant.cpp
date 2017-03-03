// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLConstant.h"
#include "KLType.h"
#include "KLASTManager.h"

using namespace FabricServices::ASTWrapper;

KLConstant::KLConstant(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data)
: KLCommented(klFile, nameSpace, data)
{
  m_name = getDictValue("constDecl")->getDictValue("name")->getStringData();
  m_type = getDictValue("constDecl")->getDictValue("scalarType")->getStringData();
}

KLConstant::~KLConstant()
{
}

KLDeclType KLConstant::getDeclType() const
{
  return KLDeclType_Constant;
}

bool KLConstant::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Constant)
    return true;
  return KLCommented::isOfDeclType(type);
}

const std::string & KLConstant::getName() const
{
  return m_name;
}

std::string KLConstant::getType(bool includeNameSpace) const
{
  if(includeNameSpace)
  {
    const KLType * klType = getASTManager()->getKLTypeByName(m_type.c_str(), this);
    if(klType)
      return klType->getNameSpacePrefix() + m_type;
  }
  return m_type;
}
