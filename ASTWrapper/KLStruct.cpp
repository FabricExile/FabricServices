// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLStruct.h"
#include "KLASTManager.h"

using namespace FabricServices::ASTWrapper;

KLStruct::KLStruct(const KLFile* klFile, JSONData data)
: KLType(klFile, data)
{
  const char * parentStructName = getStringDictValue("parentStructName");
  if(parentStructName)
    m_parentStructName = parentStructName;

  JSONData members = getArrayDictValue("members");
  m_isForwardDecl = members == NULL;
  if(!m_isForwardDecl)
  {
    for(uint32_t i=0;i<members->getArraySize();i++)
    {
      KLMember * member = new KLMember(klFile, members->getArrayElement(i));
      m_members.push_back(member);
    }
  }
}

KLStruct::~KLStruct()
{
  // KLMethods are deleted by the KLType destructor
  for(uint32_t i=0;i<m_members.size();i++)
    delete(m_members[i]);
}

KLDeclType KLStruct::getDeclType() const
{
  return KLDeclType_Struct;
}

bool KLStruct::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Struct)
    return true;
  return KLType::isOfDeclType(type);
}

const char * KLStruct::getKLType() const
{
  return "struct";
}

std::vector<const KLType*> KLStruct::getParents() const
{
  std::vector<const KLType*> parents; 
  if(m_parentStructName.length() > 0)
  {
    const KLType * parent = getASTManager()->getKLTypeByName(m_parentStructName.c_str(), this);
    if(parent)
    {
      parents.push_back(parent);
    }
  }
  return parents;
}

uint32_t KLStruct::getMemberCount(bool includeInherited) const
{
  if(!includeInherited)
    return m_members.size();

  std::vector<const KLType*> parents = getParents();
  uint32_t result = 0;
  for(uint32_t i=0;i<parents.size();i++)
  {
    if(parents[i]->getKLType() == std::string("struct") || parents[i]->getKLType() == std::string("object"))
    {
      const KLStruct* parentStruct = (const KLStruct*)parents[i];
      result += parentStruct->getMemberCount(true);
    }    
  }
  return result + m_members.size();
}

const KLMember * KLStruct::getMember(uint32_t index, bool includeInherited) const
{
  if(!includeInherited)
    return m_members[index];

  std::vector<const KLType*> parents = getParents();
  uint32_t offset = index;
  for(uint32_t i=0;i<parents.size();i++)
  {
    if(parents[i]->getKLType() == std::string("struct") || parents[i]->getKLType() == std::string("object")) 
    {
      const KLStruct* parentStruct = (const KLStruct*)parents[i];
      uint32_t memberCount = parentStruct->getMemberCount(true);
      if(offset < memberCount)
        return parentStruct->getMember(offset);
      offset -= memberCount;
    }    
  }
  return m_members[offset];
}

const KLMember * KLStruct::getMember(const char * name, bool includeInherited) const
{
  uint32_t count = getMemberCount(includeInherited);
  for(uint32_t i=0;i<count;i++)
  {
    const KLMember * member = getMember(i, includeInherited);
    if(member->getName() == name)
      return member;
  }
  return NULL;
}
