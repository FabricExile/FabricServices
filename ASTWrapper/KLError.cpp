// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLError.h"
#include "KLError.h"

using namespace FabricServices::ASTWrapper;

KLError::KLError(JSONData data)
{
  // printf("%s\n", data->getJSONEncoding().getStringData());
  m_fileName = data->getDictValue("filename")->getStringData();
  m_line = data->getDictValue("line")->getSInt32();
  m_column = data->getDictValue("column")->getSInt32();
  m_level = data->getDictValue("level")->getStringData();
  m_desc = data->getDictValue("desc")->getStringData();
}

KLError::~KLError()
{
}

const char * KLError::getFileName() const
{
  return m_fileName.c_str();
}

int KLError::getLine() const
{
  return m_line;
}

int KLError::getColumn() const
{
  return m_column;
}

const char * KLError::getLevel() const
{
  return m_level.c_str();
}

const char * KLError::getDesc() const
{
  return m_desc.c_str();
}

