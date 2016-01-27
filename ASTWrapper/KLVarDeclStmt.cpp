// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLVarDeclStmt.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLVarDeclStmt::KLVarDeclStmt(const KLFile* klFile, JSONData data, KLStmt * parent)
: KLStmt(klFile, data, parent)
{
  m_baseType = getStringDictValue("baseType");

  JSONData varDecls = getArrayDictValue("varDecls");
  if(varDecls)
  {
    for(uint32_t i=0;i<varDecls->getArraySize();i++)
    {
      JSONData varDecl = varDecls->getArrayElement(i);
      m_names.push_back(varDecl->getDictValue("name")->getStringData());
      m_arrayModifiers.push_back(varDecl->getDictValue("arrayModifier")->getStringData());
    }
  }
}

KLDeclType KLVarDeclStmt::getDeclType() const
{
  return KLDeclType_VarDeclStmt;
}

bool KLVarDeclStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_VarDeclStmt)
    return true;
  return KLStmt::isOfDeclType(type);
}

std::string KLVarDeclStmt::getBaseType() const
{
  return m_baseType;
}

uint32_t KLVarDeclStmt::getCount() const
{
  return m_names.size();
}

const std::string & KLVarDeclStmt::getName(uint32_t index) const
{
  return m_names[index];
}

const std::string & KLVarDeclStmt::getArrayModifier(uint32_t index) const
{
  return m_arrayModifiers[index];
}
