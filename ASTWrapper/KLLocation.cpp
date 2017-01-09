// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLLocation.h"

using namespace FabricServices::ASTWrapper;

KLLocation::KLLocation(JSONData data)
{
  m_line = data->getDictValue("line")->getSInt32();
  m_column = data->getDictValue("column")->getSInt32();
  m_endLine = data->getDictValue("endLine")->getSInt32();
  m_endColumn = data->getDictValue("endColumn")->getSInt32() + 1;
}

KLLocation::~KLLocation()
{
}

uint32_t KLLocation::getLine() const
{
  return m_line;
}

uint32_t KLLocation::getColumn() const
{
  return m_column;
}

uint32_t KLLocation::getEndLine() const
{
  return m_endLine;
}

uint32_t KLLocation::getEndColumn() const
{
  return m_endColumn;
}

