// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLConditionalStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLConditionalStmt::KLConditionalStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent)
: KLStmt(klFile, nameSpace, data, parent)
{
  JSONData trueStatement = getDictValue("trueStatement");
  if(trueStatement)
    constructChild(trueStatement);
  JSONData falseStatement = getDictValue("falseStatement");
  if(falseStatement)
    constructChild(falseStatement);
}

KLDeclType KLConditionalStmt::getDeclType() const
{
  return KLDeclType_ConditionalStmt;
}

bool KLConditionalStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_ConditionalStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}

const KLStmt * KLConditionalStmt::getTrueStatement() const
{
  if(getChildCount() > 0)
    return getChild(0);
  return NULL;
}

const KLStmt * KLConditionalStmt::getFalseStatement() const
{
  if(getChildCount() > 1)
    return getChild(1);
  return NULL;
}

