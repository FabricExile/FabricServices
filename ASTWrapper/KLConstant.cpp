// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLConstant.h"

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

const std::string & KLConstant::getType() const
{
  return m_type;
}
