// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLOperator.h"

using namespace FabricServices::ASTWrapper;

KLOperator::KLOperator(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data)
: KLFunction(klFile, nameSpace, data)
{
}

KLOperator::~KLOperator()
{
}

KLDeclType KLOperator::getDeclType() const
{
  return KLDeclType_Operator;
}

bool KLOperator::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Operator)
    return true;
  return KLFunction::isOfDeclType(type);
}

const char * KLOperator::getKLType() const
{
  return "operator";
}
