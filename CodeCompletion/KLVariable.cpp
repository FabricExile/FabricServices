// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLVariable.h"

using namespace FabricServices::CodeCompletion;

KLVariable::KLVariable(const std::string & name, const std::string & type)
:KLTypeDesc(type)
{
  m_name = name;
}

KLVariable::KLVariable(const std::string & name, const std::string & type, const std::string & arrayModifier)
:KLTypeDesc(type, arrayModifier)
{
  m_name = name;
}

KLVariable::~KLVariable()
{
}

const std::string & KLVariable::getName() const
{
  return m_name;
}
