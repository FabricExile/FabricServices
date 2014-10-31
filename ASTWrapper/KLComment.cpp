// Copyright 2010-2014 Fabric Engine Inc. All rights reserved.

#include "KLComment.h"

using namespace FabricCore;
using namespace ASTWrapper;

KLComment::KLComment(const Variant * data)
: KLDecl(data)
{
}

bool KLComment::isInternal() const
{
  return true;
}
