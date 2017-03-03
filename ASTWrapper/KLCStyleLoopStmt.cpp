// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLCStyleLoopStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLCStyleLoopStmt::KLCStyleLoopStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent)
: KLStmt(klFile, nameSpace, data, parent)
{
  JSONData body = getDictValue("body");
  if(body)
    constructChild(body);
}

KLDeclType KLCStyleLoopStmt::getDeclType() const
{
  return KLDeclType_CStyleLoopStmt;
}

bool KLCStyleLoopStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_CStyleLoopStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}
