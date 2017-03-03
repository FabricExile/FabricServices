// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLCaseStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLCaseStmt::KLCaseStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent)
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

KLDeclType KLCaseStmt::getDeclType() const
{
  return KLDeclType_CaseStmt;
}

bool KLCaseStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_CaseStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}
