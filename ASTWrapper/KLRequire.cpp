// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLRequire.h"

using namespace FabricServices::ASTWrapper;

KLRequire::KLRequire(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data)
: KLCommented(klFile, nameSpace, data)
{
  JSONData require = getArrayDictValue("requires")->getArrayElement(0);
  m_requiredExtension = require->getDictValue("name")->getStringData();
  m_versionRange = require->getDictValue("versionRange")->getStringData();
}

KLRequire::~KLRequire()
{
}

KLDeclType KLRequire::getDeclType() const
{
  return KLDeclType_Require;
}

bool KLRequire::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Require)
    return true;
  return KLCommented::isOfDeclType(type);
}

const std::string & KLRequire::getRequiredExtension() const
{
  return m_requiredExtension;
}

const std::string & KLRequire::getVersionRange() const
{
  return m_versionRange;
}

