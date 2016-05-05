// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLASTClient.h"
#include "KLASTManager.h"
#include "KLTypeOp.h"

#include <FTL/Env.h>
#include <FTL/FS.h>
#include <FTL/StrFilterWhitespace.h>
#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;

KLASTManager * KLASTManager::s_manager = NULL;
uint32_t KLASTManager::s_managerRefs = 0;

KLASTManager::KLASTManager(const FabricCore::Client * client)
{
  if(client)
    m_client = *client;
  m_maxDeclId = 0;
  m_isUpdatingASTClients = false;
  m_autoLoadExtensions = false;
}

KLASTManager::~KLASTManager()
{
  for(uint32_t i=0;i<m_extensions.size();i++)
    delete(m_extensions[i]);

  m_isUpdatingASTClients = true;
  for(uint32_t i=0;i<m_astClients.size();i++)
    m_astClients[i]->setASTManager(NULL);
}

KLASTManager * KLASTManager::retainGlobalManager(const FabricCore::Client * client)
{
  if(s_manager == NULL)
    s_manager = new KLASTManager(client);
  s_managerRefs++;
  return s_manager;
}

void KLASTManager::releaseGlobalManager()
{
  if(s_managerRefs > 0)
  {
    s_managerRefs--;
    if(s_managerRefs == 0 && s_manager != NULL)
    {
      delete(s_manager);
      s_manager = NULL;
    }
  }
}

const FabricCore::Client* KLASTManager::getClient() const
{
  return &m_client;
}

void KLASTManager::registerASTClient(KLASTClient * client)
{
  if(m_isUpdatingASTClients)
    return;
  m_astClients.push_back(client);
}

void KLASTManager::unregisterASTClient(KLASTClient * client)
{
  if(m_isUpdatingASTClients)
    return;

  size_t index = m_astClients.size();
  for(size_t i=0;i<m_astClients.size();i++)
  {
    if(m_astClients[i] == client)
    {
      index = i;
      break;
    }
  }

  if(index != m_astClients.size())
    m_astClients.erase(m_astClients.begin() + index);
}

void KLASTManager::onExtensionLoaded(const KLExtension * extension)
{
  for(size_t i=0;i<m_astClients.size();i++)
  {
    m_astClients[i]->onExtensionLoaded(extension);
  }
  onASTChanged();
}

void KLASTManager::onExtensionParsed(const KLExtension * extension)
{
  for(size_t i=0;i<m_astClients.size();i++)
  {
    m_astClients[i]->onExtensionParsed(extension);
  }
  onASTChanged();
}

void KLASTManager::onFileLoaded(const KLFile * file)
{
  for(size_t i=0;i<m_astClients.size();i++)
  {
    m_astClients[i]->onFileLoaded(file);
  }
  onASTChanged();
}

void KLASTManager::onFileParsed(const KLFile * file)
{
  for(size_t i=0;i<m_astClients.size();i++)
  {
    m_astClients[i]->onFileParsed(file);
  }
  onASTChanged();
}

void KLASTManager::onASTChanged()
{
  for(size_t i=0;i<m_astClients.size();i++)
  {
    m_astClients[i]->onASTChanged();
  }
}

bool KLASTManager::getAutoLoadExtensions() const
{
  return m_autoLoadExtensions;
}

void KLASTManager::setAutoLoadExtensions(bool state)
{
  m_autoLoadExtensions = state;
}

const KLExtension* KLASTManager::loadExtension(const char * name, const char * jsonContent, uint32_t numKlFiles, const char ** klContent, FabricCore::DFGExec *dfgExec)
{
  KLExtension * extension = new KLExtension(this, name, jsonContent, numKlFiles, klContent, dfgExec);
  m_extensions.push_back(extension);
  onExtensionLoaded(extension);
  extension->parse();
  onExtensionParsed(extension);
  return extension;
}

const KLExtension* KLASTManager::loadExtension(const char * jsonFilePath, FabricCore::DFGExec *dfgExec)
{
  KLExtension * extension = new KLExtension(this, jsonFilePath, dfgExec);
  m_extensions.push_back(extension);
  onExtensionLoaded(extension);
  extension->parse();
  onExtensionParsed(extension);
  return extension;
}

void KLASTManager::loadAllExtensionsInFolder(const char * extensionFolder, bool parseExtensions)
{
  std::vector<std::string> folders;
  folders.push_back(extensionFolder);

  std::vector<std::string> additionalFolders;

  for(uint32_t i=0;i<folders.size();i++)
  {
    std::string const &folder = folders[i];

    std::vector<std::string> entries;
    if ( !FTL::FSDirAppendEntries( folder, entries ) )
      continue;

    for ( std::vector<std::string>::const_iterator it = entries.begin();
      it != entries.end(); ++it )
    {
      std::string const &entry = *it;
      std::string entryPath = FTL::PathJoin( folder, entry );
      FTL::FSStatInfo entryStatInfo;
      if ( !FTL::FSStat( entryPath, entryStatInfo ) )
        continue;
      switch ( entryStatInfo.type )
      {
        case FTL::FSStatInfo::Dir:
          additionalFolders.push_back( entryPath );
          break;

        case FTL::FSStatInfo::File:
          if ( entry.length() >= 9
              && entry.substr(entry.length()-9, 9) == ".fpm.json" )
          {
            try
            {
              KLExtension * extension = new KLExtension(this, entryPath.c_str(), NULL);
              onExtensionLoaded(extension);
              m_extensions.push_back(extension);
            }
            catch(FabricCore::Exception e)
            {
              printf("[KLASTManager] Ignoring extension '%s': '%s'.\n", entryPath.c_str(), e.getDesc_cstr());
            }
          }
          break;

        default:
          break;
      }
    }
  }

  for ( std::vector<std::string>::const_iterator it = additionalFolders.begin();
    it != additionalFolders.end(); ++it )
  {
    loadAllExtensionsInFolder( it->c_str(), false );
  }

  if(parseExtensions)
  {
    for(uint32_t i=0;i<m_extensions.size();i++)
    {
      KLExtension * klExtension = (KLExtension *)m_extensions[i];
      klExtension->parse();
      onExtensionParsed(klExtension);
    }
  }

}

bool KLASTManager::loadAllExtensionsFromExtsPath(bool parseExtensions)
{
  if(m_extensions.size() >  0)
    return false;

  std::vector<std::string> folders;
  if ( !FTL::EnvGetList( "FABRIC_EXTS_PATH", folders ) )
    return false;

  for(size_t i=0;i<folders.size();i++)
  {
    loadAllExtensionsInFolder(folders[i].c_str(), false);
  }

  if(parseExtensions)
  {
    for(uint32_t i=0;i<m_extensions.size();i++)
    {
      KLExtension * klExtension = (KLExtension *)m_extensions[i];
      klExtension->parse();
      onExtensionParsed(klExtension);
    }
  }

  return m_extensions.size() > 0;
}

const KLExtension* KLASTManager::loadExtensionFromFolder(
  const char * name,
  std::string const &folder
  )
{
  std::vector<std::string> entries;
  FTL::FSDirAppendEntries( folder, entries );

  KLExtension const *result = NULL;
  for ( std::vector<std::string>::const_iterator it = entries.begin();
    !result && it != entries.end(); ++it )
  {
    std::string const &entry = *it;
    std::string entryPath = FTL::PathJoin( folder, entry );

    FTL::FSStatInfo statInfo;
    if ( !FSStat( entryPath, statInfo ) )
      continue;

    switch ( statInfo.type )
    {
      case FTL::FSStatInfo::Dir:
        result = loadExtensionFromFolder( name, entryPath );
        break;

      case FTL::FSStatInfo::File:
        if ( entry == std::string(name) + ".fpm.json" )
        {
          try
          {
            KLExtension *klExtension =
              new KLExtension(this, entryPath.c_str(), NULL);
            onExtensionLoaded(klExtension);
            m_extensions.push_back(klExtension);
            klExtension->parse();
            onExtensionParsed(klExtension);
            result = klExtension;
          }
          catch(FabricCore::Exception e)
          {
            printf("[KLASTManager] Ignoring extension '%s': '%s'.\n", entryPath.c_str(), e.getDesc_cstr());
          }
        }
        break;

      default:
        break;
    }
  }
  return result;
}

const KLExtension* KLASTManager::loadExtensionFromFolders(
  const char * name,
  std::vector<std::string> const &folders
  )
{
  KLExtension const *result = NULL;
  for(uint32_t i=0;i<folders.size();i++)
  {
    std::string const &folder = folders[i];
    result = loadExtensionFromFolder( name, folder );
    if ( result )
      break;
  }
  return result;
}

const KLExtension* KLASTManager::loadExtensionFromExtsPath(const char * name)
{
  if(getExtension(name))
    return NULL;

  std::vector<std::string> folders;
  if ( !FTL::EnvGetList( "FABRIC_EXTS_PATH", folders ) )
    return NULL;

  return loadExtensionFromFolders( name, folders );
}

bool KLASTManager::removeExtension(const char * name, const char * versionRequirement)
{
  const KLExtension * extension = getExtension(name, versionRequirement);
  if(extension)
  {
    for(size_t i=0;i<m_extensions.size();i++)
    {
      if(m_extensions[i] == extension)
      {
        delete(m_extensions[i]);
        m_extensions.erase(m_extensions.begin() + i);
        return true;
      }
    }
  }
  return false;
}

const KLFile* KLASTManager::loadSingleKLFile(const char * klFileName, const char * klContent, FabricCore::DFGExec *dfgExec)
{
  loadAllExtensionsFromExtsPath(false);

  const KLExtension * extension = getExtension(klFileName);
  if(!extension)
  {
    std::string json = "{\n\"code\": \"";
    json += klFileName;
    json += "\"\n}\n";
    extension = loadExtension(klFileName, json.c_str(), 1, &klContent, dfgExec);
    if(!extension)
      return NULL;
    return extension->getFiles()[0];
  }
  KLFile* file = (KLFile*)extension->getFiles()[0];
  file->updateKLCode(klContent);
  return file;
}

std::vector<const KLExtension*> KLASTManager::getExtensions() const
{
  return m_extensions;
}

std::vector<const KLRequire*> KLASTManager::getRequires() const
{
  std::vector<const KLRequire*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLRequire*> singleResult = m_extensions[i]->getRequires();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLAlias*> KLASTManager::getAliases() const
{
  std::vector<const KLAlias*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLAlias*> singleResult = m_extensions[i]->getAliases();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLConstant*> KLASTManager::getConstants() const
{
  std::vector<const KLConstant*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLConstant*> singleResult = m_extensions[i]->getConstants();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLType*> KLASTManager::getTypes() const
{
  std::vector<const KLType*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLType*> singleResult = m_extensions[i]->getTypes();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLFunction*> KLASTManager::getFunctions() const
{
  std::vector<const KLFunction*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLFunction*> singleResult = m_extensions[i]->getFunctions();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLInterface*> KLASTManager::getInterfaces() const
{
  std::vector<const KLInterface*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLInterface*> singleResult = m_extensions[i]->getInterfaces();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLStruct*> KLASTManager::getStructs() const
{
  std::vector<const KLStruct*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLStruct*> singleResult = m_extensions[i]->getStructs();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLObject*> KLASTManager::getObjects() const
{
  std::vector<const KLObject*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLObject*> singleResult = m_extensions[i]->getObjects();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLOperator*> KLASTManager::getOperators() const
{
  std::vector<const KLOperator*> result;
  for(uint32_t i=0;i<m_extensions.size();i++)
  {
    std::vector<const KLOperator*> singleResult = m_extensions[i]->getOperators();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

const KLType* KLASTManager::getKLTypeByName(const char * name, const KLDecl* decl) const
{
  if(decl)
    return getKLTypeByName(name, decl->getKLFile());

  // if we don't have it in the provided extension, check the global map
  // go backwards since we want to check highest extension versions first
  std::vector<const KLType*> types = getTypes();
  for(long int i=types.size()-1;i>=0;i--)
  {
    if(types[i]->getName() == name)
      return types[i];
  }

  return NULL;
}

const KLType* KLASTManager::getKLTypeByName(const char * name, const KLFile* file) const
{
  // first check withinour own extension
  if(file)
  {
    std::vector<const KLType*> types = file->getExtension()->getTypes();
    for(uint32_t i=0;i<types.size();i++)
    {
      if(types[i]->getName() == name)
        return types[i];
    }

    // if the type isn't inside our own extension,
    // get all requires and find the corresponding matching extensions
    std::vector<const KLRequire*> requires = file->getExtension()->getRequires();
    for(uint32_t i=0;i<requires.size();i++)
    {
      const KLExtension* extension = getExtension(requires[i]);
      if(extension)
      {
        std::vector<const KLType*> types = extension->getTypes();
        for(uint32_t j=0;j<types.size();j++)
        {
          if(types[j]->getName() == name)
            return types[j];
        }
      }
    }
  }
  
  // if we don't have it in the provided extension, check the global map
  // go backwards since we want to check highest extension versions first
  std::vector<const KLType*> types = getTypes();
  for(long int i=types.size()-1;i>=0;i--)
  {
    if(types[i]->getName() == name)
      return types[i];
  }

  return NULL;
}

const KLType* KLASTManager::getKLTypeByName(const char * name, const char * extension, const char * versionRequirement) const
{
  const KLExtension* ext = getExtension(extension, versionRequirement);
  if(ext)
  {
    std::vector<const KLType*> types = ext->getTypes();
    for(uint32_t j=0;j<types.size();j++)
    {
      if(types[j]->getName() == name)
        return types[j];
    }
  }
  return NULL;
}

const KLExtension* KLASTManager::getExtension(const char * name, const char * versionRequirement) const
{
  std::map<KLExtension::Version, const KLExtension*> extensions;
  for(uint32_t i=0;i<m_extensions.size();i++) {
    if(std::string(m_extensions[i]->getName()) != name)
      continue;
    if(extensions.find(m_extensions[i]->getVersion()) != extensions.end())
      continue;
    extensions.insert(std::pair<KLExtension::Version, const KLExtension*>(m_extensions[i]->getVersion(), m_extensions[i]));
  }
  
  std::string r = FTL::StrFilterWhitespace( versionRequirement );

  KLTypeOp::OpType op = KLTypeOp::OpType_Equal;
  if(r.substr(0, 2) == "==")
  {
    op = KLTypeOp::OpType_Equal;
    r = r.substr(2, 10000);
  }
  else if(r.substr(0, 2) == "!=")
  {
    op = KLTypeOp::OpType_NotEqual;
    r = r.substr(2, 10000);
  }
  else if(r.substr(0, 2) == ">=")
  {
    op = KLTypeOp::OpType_GreaterEquals;
    r = r.substr(2, 10000);
  }
  else if(r.substr(0, 2) == "<=")
  {
    op = KLTypeOp::OpType_LessEquals;
    r = r.substr(2, 10000);
  }
  else if(r.substr(0, 1) == ">")
  {
    op = KLTypeOp::OpType_GreaterThan;
    r = r.substr(1, 10000);
  }
  else if(r.substr(0, 1) == "<")
  {
    op = KLTypeOp::OpType_LessThan;
    r = r.substr(1, 10000);
  }

  KLExtension::Version rVersion;
  if(r.length() > 1)
  {
    std::vector<std::string> strParts;
    FTL::StrSplit<'.'>( r, strParts, true /* strict */ );

    std::vector<int> intParts;
    for(uint32_t i=0;i<strParts.size();i++)
      intParts.push_back(atoi(strParts[i].c_str()));

    if(intParts.size() < 1) intParts.push_back(1);
    if(intParts.size() < 2) intParts.push_back(0);
    if(intParts.size() < 3) intParts.push_back(0);

    rVersion.major = intParts[0];
    rVersion.minor = intParts[1];
    rVersion.revision = intParts[2];
  }

  std::map<KLExtension::Version, const KLExtension*>::const_reverse_iterator it;
  for(it = extensions.rbegin(); it != extensions.rend(); it++)
  {
    if(r == "*")
      return it->second;

    switch(op)
    {
      case KLTypeOp::OpType_Equal:
      {
        if(it->second->getVersion() == rVersion)
          return it->second;
        break;
      }
      case KLTypeOp::OpType_NotEqual:
      {
        if(it->second->getVersion() != rVersion)
          return it->second;
        break;
      }
      case KLTypeOp::OpType_GreaterThan:
      {
        if(it->second->getVersion() > rVersion)
          return it->second;
        break;
      }
      case KLTypeOp::OpType_LessThan:
      {
        if(it->second->getVersion() < rVersion)
          return it->second;
        break;
      }
      case KLTypeOp::OpType_GreaterEquals:
      {
        if(it->second->getVersion() > rVersion || it->second->getVersion() == rVersion)
          return it->second;
        break;
      }
      case KLTypeOp::OpType_LessEquals:
      {
        if(it->second->getVersion() < rVersion || it->second->getVersion() == rVersion)
          return it->second;
        break;
      }
      default:
        break;
    }
  }

  return NULL;
}

const KLExtension* KLASTManager::getExtension(const KLRequire* require) const
{
  return getExtension(require->getRequiredExtension().c_str(), require->getVersionRange().c_str());
}

uint32_t KLASTManager::generateDeclId()
{
  return m_maxDeclId++;
}
