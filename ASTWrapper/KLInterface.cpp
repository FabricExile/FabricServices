// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLInterface.h"

using namespace FabricServices::ASTWrapper;

KLInterface::KLInterface(const KLFile* klFile, JSONData data)
: KLType(klFile, data)
{

  JSONData members = getArrayDictValue("members");
  m_isForwardDecl = members == NULL;
  if(!m_isForwardDecl)
  {
    for(uint32_t i=0;i<members->getArraySize();i++)
    {
      KLMethod * m = new KLMethod(klFile, members->getArrayElement(i), getName());
      pushMethod(m);
    }
  }
}

KLInterface::~KLInterface()
{
  // KLMethods are deleted by the KLType destructor
}

KLDeclType KLInterface::getDeclType() const
{
  return KLDeclType_Interface;
}

bool KLInterface::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Interface)
    return true;
  return KLType::isOfDeclType(type);
}

const char * KLInterface::getKLType() const
{
  return "interface";
}

std::vector<const KLType*> KLInterface::getParents() const
{
  return std::vector<const KLType*>(); 
}
