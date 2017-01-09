// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLObject.h"
#include "KLASTManager.h"

using namespace FabricServices::ASTWrapper;

KLObject::KLObject(const KLFile* klFile, JSONData data)
: KLStruct(klFile, data)
{
  JSONData parentsAndInterfaces = getArrayDictValue("parentsAndInterfaces");
  if(parentsAndInterfaces)
  {
    for(uint32_t i=0;i<parentsAndInterfaces->getArraySize();i++)
    {
      m_parentsAndInterfaces.push_back(parentsAndInterfaces->getArrayElement(i)->getStringData());
    }
  }
}

KLObject::~KLObject()
{
  // KLMethods are deleted by the KLType destructor
}

KLDeclType KLObject::getDeclType() const
{
  return KLDeclType_Object;
}

bool KLObject::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Object)
    return true;
  return KLStruct::isOfDeclType(type);
}

const char * KLObject::getKLType() const
{
  return "object";
}

std::vector<const KLType*> KLObject::getParents() const
{
  std::vector<const KLType*> parents; 
  for(uint32_t i=0;i<m_parentsAndInterfaces.size();i++)
  {
    const KLType * parent = getASTManager()->getKLTypeByName(m_parentsAndInterfaces[i].c_str(), this);
    if(parent)
    {
      parents.push_back(parent);
    }
  }
  return parents;
}
