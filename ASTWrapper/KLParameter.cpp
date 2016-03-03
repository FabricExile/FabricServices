// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLParameter.h"

#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;

KLParameter::KLParameter(const KLFile* klFile, JSONData data)
: KLDecl(klFile, data)
{
  m_usage = getStringDictValue("usage");
  m_name = getStringDictValue("name");
  m_type = getStringDictValue("typeUserName");
}

KLParameter::~KLParameter()
{
}

KLDeclType KLParameter::getDeclType() const
{
  return KLDeclType_Parameter;
}

bool KLParameter::isOfDeclType(KLDeclType type) const
{
  return type == KLDeclType_Parameter;
}

const std::string & KLParameter::getUsage() const
{
  return m_usage;
}

const std::string & KLParameter::getName() const
{
  return m_name;
}

const std::string & KLParameter::getType() const
{
  return m_type;
}

std::string KLParameter::getTypeNoArray() const
{
  std::vector<std::string> parts;
  FTL::StrSplit<'['>( m_type, parts );
  std::string type = parts[0];
  if(type.substr(type.length()-2, 2) == "<>")
    type = type.substr(0, type.length()-2);
  return type;
}

std::string KLParameter::getTypeArraySuffix() const
{
  if(m_type.substr(m_type.length()-2, 2) == "<>")
    return "<>";
  if(m_type.substr(m_type.length()-1, 1) == "]")
  {
    std::vector<std::string> parts;
    FTL::StrSplit<'['>( m_type, parts );
    return "[" + parts[1];
  }
  return "";
}

