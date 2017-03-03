// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLCompoundStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLCompoundStmt::KLCompoundStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent)
: KLStmt(klFile, nameSpace, data, parent)
{
  JSONData statements = getArrayDictValue("statements");
  if(statements)
  {
    for(uint32_t i=0;i<statements->getArraySize();i++)
    {
      constructChild(statements->getArrayElement(i));
    }
  }
}

KLDeclType KLCompoundStmt::getDeclType() const
{
  return KLDeclType_CompoundStmt;
}

bool KLCompoundStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_CompoundStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}
