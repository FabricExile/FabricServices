// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLExtension.h"

#include <FTL/FS.h>
#include <FTL/Path.h>
#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;

bool KLExtension::Version::operator <(const KLExtension::Version & other) const
{
  if(major < other.major)
    return true;
  if(minor < other.minor)
    return true;
  return revision < other.revision;
}

bool KLExtension::Version::operator >(const KLExtension::Version & other) const
{
  if(major > other.major)
    return true;
  if(minor > other.minor)
    return true;
  return revision > other.revision;
}

bool KLExtension::Version::operator ==(const KLExtension::Version & other) const
{
  return major == other.major && minor == other.minor && revision == other.revision;
}

bool KLExtension::Version::operator !=(const KLExtension::Version & other) const
{
  return major != other.major || minor != other.minor || revision != other.revision;
}

KLExtension::KLExtension(const KLASTManager* astManager, const char * jsonFilePath)
{
  m_astManager = (KLASTManager*)astManager;

  std::string jsonFilePathReplaced = jsonFilePath;
  std::pair<FTL::StrRef, FTL::StrRef> jsonFilePathSplit =
    FTL::PathSplit( jsonFilePathReplaced );
  m_name = jsonFilePathSplit.second;
  if(m_name.length() > 9)
  {
    if(m_name.substr(m_name.length()-9, 9) == ".fpm.json")
    {
      m_name = m_name.substr(0, m_name.length()-9);
    }
  }

  m_filePath = jsonFilePath;

  FILE * jsonFile = fopen(jsonFilePath, "rb");
  if(!jsonFile)
  {
    std::string message = "KLExtension: jsonFilePath '";
    message += jsonFilePath;
    message += "' does not exist.";
    throw(FabricCore::Exception(message.c_str()));
  }

  fseek( jsonFile, 0, SEEK_END );
  int jsonFileSize = ftell( jsonFile );
  rewind( jsonFile );

  char * jsonFileBuffer = (char*) malloc(jsonFileSize + 1);
  jsonFileBuffer[jsonFileSize] = '\0';

  fread(jsonFileBuffer, jsonFileSize, 1, jsonFile);
  fclose(jsonFile);

  std::string jsonContent = jsonFileBuffer;
  free(jsonFileBuffer);

  FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonContent.c_str());
  
  std::vector<std::string> klFileRelPaths = extractKLFilePaths(&jsonVar, m_name.c_str());
  std::vector<std::string> klContent;
  for(uint32_t i=0;i<klFileRelPaths.size();i++)
  {
    std::string klFilePath = klFileRelPaths[i];
    if ( !FTL::PathIsAbsolute( klFilePath ) )
      klFilePath = FTL::PathJoin( jsonFilePathSplit.first, klFileRelPaths[i] );

    if ( !FTL::FSExists( klFilePath ) )
    {
      std::string message = "KLExtension: '" + m_name + "' uses a non existing KL file '";
      message += klFileRelPaths[i];
      message += "'.";
      throw(FabricCore::Exception(message.c_str()));
    }

    klContent.resize( klContent.size() + 1 );
    std::string &fileContent = klContent.back();

    FILE * klFile = fopen(klFilePath.c_str(), "rb");
    for (;;)
    {
      size_t oldSize = fileContent.size();
      fileContent.resize( oldSize + 16384 );
      size_t readResult = 
        fread(&fileContent[oldSize], 1, 16384, klFile);
      fileContent.resize( oldSize + readResult );
      if ( readResult < 16384 )
        break;
    }
    fclose(klFile);
  }

  std::vector<const char *> klContentCStr;
  for(uint32_t i=0;i<klFileRelPaths.size();i++)
  {
    klContentCStr.push_back(klContent[i].c_str());
  }

  init(jsonContent.c_str(), klContentCStr.size(), &klContentCStr[0]);
}

KLExtension::KLExtension(const KLASTManager* astManager, const char * name, const char * jsonContent, uint32_t numKLFiles, const char ** klContent)
{
  m_astManager = (KLASTManager*)astManager;
  m_name = name;
  m_filePath = m_name + ".fpm.json";
  init(jsonContent, numKLFiles, klContent);
}

KLExtension::~KLExtension()
{
  for(uint32_t i=0;i<m_files.size();i++)
    delete(m_files[i]);
}

void KLExtension::init(const char * jsonContent, uint32_t numKLFiles, const char ** klContent)
{
  m_parsed = false;

  FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonContent);

  if(!jsonVar.isDict())
  {
    std::string message = "KLExtension: The extension '";
    message += m_name;
    message += "' does not contain a json dictionary.";
    throw(FabricCore::Exception(message.c_str()));
  }

  JSONData versionVar = jsonVar.getDictValue("version");
  if(versionVar)
  {
    if(versionVar->isString())
    {
      std::string version = versionVar->getStringData();
      std::vector<std::string> strParts;
      FTL::StrSplit<'.'>( version, strParts );

      std::vector<int> intParts;
      for(uint32_t i=0;i<strParts.size();i++)
        intParts.push_back(atoi(strParts[i].c_str()));

      if(intParts.size() < 1) intParts.push_back(1);
      if(intParts.size() < 2) intParts.push_back(0);
      if(intParts.size() < 3) intParts.push_back(0);

      m_version.major = intParts[0];
      m_version.minor = intParts[1];
      m_version.revision = intParts[2];
    }
    else
    {
      std::string message = "KLExtension: The extension '";
      message += m_name;
      message += "' contains an invalid version dictionary value (not string).";
      throw(FabricCore::Exception(message.c_str()));
    }
  }
  else
  {
    m_version.major = 1;
    m_version.minor = 0;
    m_version.revision = 0;
  }

  std::vector<std::string> klFilePaths = extractKLFilePaths(&jsonVar, m_name.c_str());
  if(klFilePaths.size() != numKLFiles)
  {
    std::string message = "KLExtension: The number of provided KL sources for extension '";
    message += m_name;
    message += "' does not match the code dictionary value in json.";
    throw(FabricCore::Exception(message.c_str()));
  }

  for(uint32_t i=0;i<klFilePaths.size();i++)
  {
    KLFile * klFile = new KLFile(this, klFilePaths[i].c_str(), klContent[i]);
    getASTManager()->onFileLoaded(klFile);
    m_files.push_back(klFile);
  }
}

void KLExtension::parse()
{
  if(m_parsed)
    return;

  m_parsed = true;

  for(uint32_t i=0;i<m_files.size();i++)
  {
    KLFile * klFile = (KLFile *)m_files[i];
    klFile->parse();
    getASTManager()->onFileParsed(klFile);
  }
}

std::vector<std::string> KLExtension::extractKLFilePaths(JSONData data, const char * extensionName)
{
  std::vector<std::string> result;

  if(!data->isDict())
  {
    std::string message = "KLExtension: The extension '";
    message += extensionName;
    message += "' does not contain a json dictionary.";
    throw(FabricCore::Exception(message.c_str()));
  }

  JSONData codeVar = data->getDictValue("code");
  if(!codeVar)
  {
    std::string message = "KLExtension: The extension '";
    message += extensionName;
    message += "' does not contain a code dictionary value.";
    throw(FabricCore::Exception(message.c_str()));
  }

  if(codeVar->isString())
  {
    result.push_back(codeVar->getStringData());
  }
  else if(codeVar->isArray())
  {
    for(uint32_t i=0;i<codeVar->getArraySize();i++)
    {
      JSONData singleCodeVar = codeVar->getArrayElement(i);
      if(singleCodeVar->isString())
      {
        result.push_back(singleCodeVar->getStringData());
      }
      else
      {
        std::string message = "KLExtension: The extension '";
        message += extensionName;
        message += "' contains a code array with an invalid element.";
        throw(FabricCore::Exception(message.c_str()));
      }
    }
  }
  else
  {
    std::string message = "KLExtension: The extension '";
    message += extensionName;
    message += "' contains an invalid code dictionary value (neither string nor array).";
    throw(FabricCore::Exception(message.c_str()));
  }

  return result;
}

const KLASTManager * KLExtension::getASTManager() const
{
  return m_astManager;
}

KLASTManager * KLExtension::getASTManager() 
{
  return m_astManager;
}

const char * KLExtension::getName() const
{
  return m_name.c_str();
}

const char * KLExtension::getFilePath() const
{
  return m_filePath.c_str();
}

const KLExtension::Version & KLExtension::getVersion() const
{
  return m_version;
}

std::vector<const KLFile*> KLExtension::getFiles() const
{
  return m_files;
}

std::vector<const KLRequire*> KLExtension::getRequires() const
{
  std::vector<const KLRequire*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLRequire*> singleResult = m_files[i]->getRequires();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLAlias*> KLExtension::getAliases() const
{
  std::vector<const KLAlias*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLAlias*> singleResult = m_files[i]->getAliases();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLConstant*> KLExtension::getConstants() const
{
  std::vector<const KLConstant*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLConstant*> singleResult = m_files[i]->getConstants();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLType*> KLExtension::getTypes() const
{
  std::vector<const KLType*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLType*> singleResult = m_files[i]->getTypes();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLFunction*> KLExtension::getFunctions() const
{
  std::vector<const KLFunction*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLFunction*> singleResult = m_files[i]->getFunctions();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLMethod*> KLExtension::getMethods() const
{
  std::vector<const KLMethod*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLMethod*> singleResult = m_files[i]->getMethods();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLInterface*> KLExtension::getInterfaces() const
{
  std::vector<const KLInterface*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLInterface*> singleResult = m_files[i]->getInterfaces();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLStruct*> KLExtension::getStructs() const
{
  std::vector<const KLStruct*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLStruct*> singleResult = m_files[i]->getStructs();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLObject*> KLExtension::getObjects() const
{
  std::vector<const KLObject*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLObject*> singleResult = m_files[i]->getObjects();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLOperator*> KLExtension::getOperators() const
{
  std::vector<const KLOperator*> result;
  for(uint32_t i=0;i<m_files.size();i++)
  {
    std::vector<const KLOperator*> singleResult = m_files[i]->getOperators();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

void KLExtension::storeForwardDeclComments(const KLType * klType)
{
  std::vector<std::string> content = klType->getComments()->getContentAsVector();
  std::map< std::string, std::vector< std::string > >::iterator it;
  it = m_forwardDeclComments.find(klType->getName());
  if(it == m_forwardDeclComments.end())
  {
    m_forwardDeclComments.insert(std::pair< std::string, std::vector< std::string > >(
      klType->getName(), content
      ));
  }
  else
  {
    it->second.insert(it->second.end(), content.begin(), content.end());
  }

  std::vector<const KLType*> types = getTypes();
  for(size_t i=0;i<types.size();i++)
  {
    if(types[i]->getName() == klType->getName())
    {
      consumeForwardDeclComments(types[i]);
      return;
    }
  }
}

void KLExtension::consumeForwardDeclComments(const KLType * klType)
{
  std::map< std::string, std::vector< std::string > >::iterator it;
  it = m_forwardDeclComments.find(klType->getName());
  if(it == m_forwardDeclComments.end())
    return;

  klType->getComments()->appendToContent(it->second);
  m_forwardDeclComments.erase(it);
}
