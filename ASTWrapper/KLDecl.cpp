// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLDecl.h"
#include "KLASTManager.h"
#include "KLExtension.h"
#include "KLFile.h"
#include "KLLocation.h"

#include <map>

using namespace FabricServices::ASTWrapper;

KLDecl::KLDecl(const KLFile* klFile, JSONData data)
{
  m_data = data;
  m_klFile = klFile;
  KLASTManager * manager = (KLASTManager *)getASTManager();
  m_id = manager->generateDeclId();

  m_location = NULL;
  if(m_data->isDict())
  {
    JSONData location = m_data->getDictValue("sourceInfo");
    if(location)
      m_location = new KLLocation(location);
  }
}

KLDecl::~KLDecl()
{
  if(m_location)
    delete(m_location);
}

uint32_t KLDecl::getID() const
{
  return m_id;
}

const KLASTManager* KLDecl::getASTManager() const
{
  return getExtension()->getASTManager();
}

const KLExtension* KLDecl::getExtension() const
{
  return getKLFile()->getExtension();
}

const KLFile* KLDecl::getKLFile() const
{
  return m_klFile;
}

const KLLocation * KLDecl::getLocation() const
{
  return m_location;
}

uint32_t KLDecl::getArraySize() const
{
  if(!m_data->isArray())
    return 0;
  return m_data->getArraySize();
}

const char * KLDecl::getStringArrayElement(uint32_t index) const
{
  JSONData value = getArrayElement(index);
  if(!value)
    return NULL;
  if(!value->isString())
    throw(FabricCore::Exception("KLDecl::getStringArrayElement called on non-string array element."));
  return value->getStringData();
}

const char * KLDecl::getStringDictValue(const char * key) const
{
  JSONData value = getDictValue(key);
  if(!value)
    return NULL;
  if(!value->isString())
    throw(FabricCore::Exception("KLDecl::getStringDictValue called on non-string dict element."));
  return value->getStringData();
}

JSONData KLDecl::getArrayElement(uint32_t index) const
{
  if(!m_data->isArray())
    throw(FabricCore::Exception("KLDecl::getArrayElement called on non-array data."));
  return m_data->getArrayElement(index);
}

JSONData KLDecl::getDictValue(const char * key) const
{
  if(!m_data->isDict())
    return NULL;
  return m_data->getDictValue(key);
}

JSONData KLDecl::getArrayDictValue(const char * key) const
{
  JSONData result = getDictValue(key);
  if(!result)
    return NULL;
  if(!result->isArray())
    throw(FabricCore::Exception("KLDecl::getArrayDictValue called on non-array dict element."));
  return result;
}
