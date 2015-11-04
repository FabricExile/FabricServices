// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "KLType.h"

#include <map>

using namespace FabricServices::ASTWrapper;

KLType::KLType(const KLFile* klFile, JSONData data)
: KLCommented(klFile, data)
{
  const char * name = getStringDictValue("name");
  if(name)
    m_name = name;
}

KLType::~KLType()
{
  for(uint32_t i=0;i<m_methods.size();i++)
  {
    delete(m_methods[i]);
  }
}

KLDeclType KLType::getDeclType() const
{
  return KLDeclType_Type;
}

bool KLType::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Type)
    return true;
  return KLCommented::isOfDeclType(type);
}

const std::string & KLType::getName() const
{
  return m_name;
}

uint32_t KLType::getMethodCount() const
{
  return m_methods.size();
}

const KLMethod * KLType::getMethod(uint32_t index) const
{
  return m_methods[index];
}

const KLMethod * KLType::getMethod(const char * labelOrName) const
{
  if(!labelOrName)
    return NULL;

  std::map<std::string, uint32_t>::const_iterator it = m_methodLabelToId.find(labelOrName);
  if(it != m_methodLabelToId.end())
    return m_methods[it->second];

  for(uint32_t i=0;i<m_methods.size();i++)
  {
    if(m_methods[i]->getName() == labelOrName)
      return m_methods[i];
    if(m_methods[i]->getLabel() == labelOrName)
      return m_methods[i];
  }
  return NULL;
}

std::vector<const KLMethod*> KLType::getMethods(
  bool includeInherited,
  bool includeInternal,
  const char * category,
  bool sorted
  ) const
{
  std::map<std::string, const KLMethod*> lookup;
  std::vector<const KLMethod*> flatList;
  for(uint32_t i=0;i<m_methods.size();i++)
  {
    if(!includeInternal && m_methods[i]->isInternal())
      continue;

    if(category)
    {
      if(m_methods[i]->getComments()->getSingleQualifier("category", category) != category)
        continue;
    }

    std::string key = m_methods[i]->getLabel();
    lookup.insert(std::pair<std::string, const KLMethod*>(key,m_methods[i]));
    flatList.push_back(m_methods[i]);
  }

  if(includeInherited)
  {
    std::vector<const KLType*> parents = getParents();
    for(uint32_t i=0;i<parents.size();i++)
    {
      std::vector<const KLMethod*> methods = parents[i]->getMethods(includeInherited, includeInternal, category, sorted);
      for(uint32_t j=0;j<methods.size();j++)
      {
        if(methods[j]->isConstructor())
          continue;
        std::string key = methods[j]->getLabel();
        if(lookup.find(key) != lookup.end())
         continue;
        lookup.insert(std::pair<std::string, const KLMethod*>(key, methods[j]));
        flatList.push_back(methods[j]);
      }
    }
  }

  if(!sorted)
    return flatList;

  std::vector<const KLMethod*> methods;
  for(std::map<std::string, const KLMethod*>::const_iterator it = lookup.begin(); it != lookup.end(); it++)
    methods.push_back(it->second);
  return methods;
}

uint32_t KLType::getTypeOpCount() const
{
  return m_typeOps.size();
}

const KLTypeOp* KLType::getTypeOp(uint32_t index) const
{
  return m_typeOps[index];
}

const KLTypeOp* KLType::getTypeOp(const char * labelOrName) const
{
  if(!labelOrName)
    return NULL;

  std::map<std::string, uint32_t>::const_iterator it = m_typeOpLabelToId.find(labelOrName);
  if(it != m_typeOpLabelToId.end())
    return m_typeOps[it->second];

  for(uint32_t i=0;i<m_typeOps.size();i++)
  {
    if(m_typeOps[i]->getName() == labelOrName)
      return m_typeOps[i];
    if(m_typeOps[i]->getLabel() == labelOrName)
      return m_typeOps[i];
  }
  return NULL;
}

std::vector<const KLTypeOp*> KLType::getTypeOps() const
{
  return m_typeOps;
}

bool KLType::pushMethod(KLMethod * method) const
{
  if(m_methodLabelToId.find(method->getLabel()) != m_methodLabelToId.end())
    return false;
  m_methodLabelToId.insert(std::pair<std::string, uint32_t>(method->getLabel(), (uint32_t)m_methods.size()));
  m_methods.push_back(method);
  return true;
}

bool KLType::pushTypeOp(KLTypeOp * typeOp) const
{
  if(m_typeOpLabelToId.find(typeOp->getLabel()) != m_typeOpLabelToId.end())
    return false;
  m_typeOpLabelToId.insert(std::pair<std::string, uint32_t>(typeOp->getLabel(), (uint32_t)m_typeOps.size()));
  m_typeOps.push_back(typeOp);
  return true;
}
