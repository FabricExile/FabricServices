// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLMethod.h"
#include "KLType.h"
#include "KLASTManager.h"

using namespace FabricServices::ASTWrapper;

KLMethod::KLMethod(const KLFile* klFile, JSONData data, const std::string & thisType)
: KLFunction(klFile, data)
{
  const char * dictThisType = getStringDictValue("thisType");
  if(dictThisType)
    m_thisType = dictThisType;
  else
    m_thisType = thisType;

  const char * thisUsage = getStringDictValue("thisUsage");
  if(thisUsage)
    m_thisUsage = thisUsage;
  else
    m_thisUsage = "in";

  m_isVirtual = -1;
}

KLMethod::~KLMethod()
{
}

KLDeclType KLMethod::getDeclType() const
{
  return KLDeclType_Method;
}

bool KLMethod::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Method)
    return true;
  return KLFunction::isOfDeclType(type);
}

const std::string & KLMethod::getThisType() const
{
  return m_thisType;
}

const std::string & KLMethod::getThisUsage() const
{
  return m_thisUsage;
}

bool KLMethod::hasUniqueName() const
{
  const KLType* thisType = getASTManager()->getKLTypeByName(m_thisType.c_str(), this);
  if(!thisType)
    return KLFunction::hasUniqueName();
  
  std::vector<const ASTWrapper::KLMethod*> methods = thisType->getMethods();

  for(size_t i=0;i<methods.size();i++)
  {
    if(methods[i] == this)
      continue;
    if(methods[i]->getName() == getName())
      return false;
  }

  return true;
}

bool KLMethod::isMethod() const
{
  return true;
}

bool KLMethod::isVirtual() const
{
  if(m_isVirtual == -1)
  {
    m_isVirtual = 0;

    const KLType* thisType = getASTManager()->getKLTypeByName(m_thisType.c_str(), this);
    if(thisType)
    {
      if(std::string(thisType->getKLType()) == "interface")
      {
        const KLMethod* method = thisType->getMethod(getLabel().c_str());
        if(method)
        {
          m_isVirtual = 1;
        }
      }
      else
      {
        // todo: this will have to implement the virtual keyword at some point
        std::vector<const KLType*> parents = thisType->getParents();
        for(uint32_t i=0;i<parents.size();i++)
        {
          std::vector<const KLType*> granParents = parents[i]->getParents();
          parents.insert(parents.end(), granParents.begin(), granParents.end());

          if(std::string(parents[i]->getKLType()) == "interface")
          {
            const KLMethod* method = parents[i]->getMethod(getLabel().c_str());
            if(method)
            {
              m_isVirtual = 1;
              break;
            }
          }
        }
      }
    }
  }
  return m_isVirtual > 0;
}

bool KLMethod::isConstructor() const
{
  return getName() == getThisType();
}

std::string KLMethod::getPrefix() const
{
  // filter out constructors / destructors
  if(m_thisType == getName() || "~" + m_thisType == getName())
    return "";

  return m_thisType + ".";
}

std::string KLMethod::getSuffix() const
{
  if(m_thisUsage == "in")
    return "?";
  return "!";
}

const KLComment * KLMethod::getComments() const
{
  const KLComment * comments = KLFunction::getComments();
  if(comments)
  {
    if(!comments->isEmpty())
      return comments;
  }

  const KLType* thisType = getASTManager()->getKLTypeByName(m_thisType.c_str(), this);
  if(thisType)
  {
    std::vector<const KLType*> parents = thisType->getParents();
    for(uint32_t i=0;i<parents.size();i++)
    {
      std::vector<const KLType*> granParents = parents[i]->getParents();
      parents.insert(parents.end(), granParents.begin(), granParents.end());

      const KLMethod* method = parents[i]->getMethod(getLabel().c_str());
      if(method)
      {
        if(method->getComments())
          return method->getComments();
      }
    }
  }
  return comments;
}

