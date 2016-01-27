// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLExprStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLExprStmt::KLExprStmt(const KLFile* klFile, JSONData data, KLStmt * parent)
: KLStmt(klFile, data, parent)
{
}

KLDeclType KLExprStmt::getDeclType() const
{
  return KLDeclType_ExprStmt;
}

bool KLExprStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_ExprStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}

