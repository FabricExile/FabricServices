// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLFile.h"
#include "KLNameSpace.h"
#include "KLLocation.h"

#include <FTL/Path.h>
#include <FTL/StrTrim.h>
#include <limits.h>

using namespace FabricServices::ASTWrapper;

KLFile::KLFile(const KLExtension* extension, const char * filePath, const char * klCode)
{
  m_extension = (KLExtension*)extension;
  m_filePath = filePath;

  std::pair<FTL::StrRef, FTL::StrRef> pathSplit =
    FTL::PathSplit( m_filePath );
  m_fileName = pathSplit.second;

  pathSplit = FTL::PathSplit( extension->getFilePath() );
  m_absFilePath = FTL::PathJoin( pathSplit.first, m_filePath );
  
  m_klCode = klCode;
  m_parsed = false;
}

void KLFile::parse()
{
  if(m_parsed)
    return;
  m_parsed = true;

  const FabricCore::Client * client = m_extension->getASTManager()->getClient();

  try
  {
    FabricCore::RTVal jsonVal;
    FabricCore::DFGExec *dfgExec = m_extension->getDFGExec();
    if ( dfgExec )
      jsonVal = dfgExec->getJSONAST(m_klCode.c_str(), false);
    else
      jsonVal = GetKLJSONAST(*client, m_fileName.c_str(), m_klCode.c_str(), false);
    std::string jsonStr = jsonVal.getStringCString();

    // printf("%s\n", jsonStr.c_str());

    FabricCore::Variant variant = FabricCore::Variant::CreateFromJSON(jsonStr.c_str());
    const FabricCore::Variant * astVariant = variant.getDictValue("ast");
    if(astVariant)
      parseJSON( astVariant );
    const FabricCore::Variant * diagnosticsVariant = variant.getDictValue("diagnostics");
    if(diagnosticsVariant)
    {
      for(uint32_t i=0;i<diagnosticsVariant->getArraySize();i++)
      {
        const FabricCore::Variant * element = diagnosticsVariant->getArrayElement(i);
        m_errors.push_back(new KLError(element));
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    throw(e);
  }
}

void KLFile::parseJSON( FabricCore::Variant const *astVariant )
{
  try
  {
    for(uint32_t i=0;i<astVariant->getArraySize();i++)
    {
      const FabricCore::Variant * element = astVariant->getArrayElement(i);
      if(!element->isDict())
        continue;
      const FabricCore::Variant * etVar = element->getDictValue("type");
      if(!etVar)
        continue;
      if(!etVar->isString())
        continue;

      std::string et = etVar->getStringData();
      if ( et == "ASTFileGlobal" )
      {
        // setup the global namespace
        KLNameSpace * e = new KLNameSpace(this, NULL, element);
        m_nameSpaces.push_back(e);
        e->parseJSON( element->getDictValue( "globalList" ) );
      }
      else
      {
        std::string message = "KLFile: Unknown AST token '"+et+"'.";
        throw(FabricCore::Exception(message.c_str(), message.length()));
        return;
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    throw(e);
  }
}

KLFile::~KLFile()
{
  clear();
  for(uint32_t i=0;i<m_errors.size();i++)
    delete(m_errors[i]);
}

void KLFile::clear()
{
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
    delete(m_nameSpaces[i]);
  m_nameSpaces.clear();
}

const KLExtension* KLFile::getExtension() const
{
  return m_extension;
}

KLExtension* KLFile::getExtensionMutable() const
{
  return m_extension;
}

const char * KLFile::getFilePath() const
{
  return m_filePath.c_str();
}

const char * KLFile::getFileName() const
{
  return m_fileName.c_str();
}

const char * KLFile::getAbsoluteFilePath() const
{
  return m_absFilePath.c_str();
}

const char * KLFile::getKLCode() const
{
  return m_klCode.c_str();
}

bool KLFile::hasErrors() const
{
  return m_errors.size() > 0;
}

std::vector<const KLRequire*> KLFile::getRequires() const
{
  std::vector<const KLRequire*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLRequire*> singleResult = m_nameSpaces[i]->getRequires();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLNameSpace*> KLFile::getNameSpaces() const
{
  std::vector<const KLNameSpace*> result;
  result.insert(result.end(), m_nameSpaces.begin(), m_nameSpaces.end());
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLNameSpace*> singleResult = m_nameSpaces[i]->getNameSpaces();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLAlias*> KLFile::getAliases() const
{
  std::vector<const KLAlias*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLAlias*> singleResult = m_nameSpaces[i]->getAliases();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLConstant*> KLFile::getConstants() const
{
  std::vector<const KLConstant*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLConstant*> singleResult = m_nameSpaces[i]->getConstants();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLType*> KLFile::getTypes() const
{
  std::vector<const KLType*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLType*> singleResult = m_nameSpaces[i]->getTypes();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLFunction*> KLFile::getFunctions() const
{
  std::vector<const KLFunction*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLFunction*> singleResult = m_nameSpaces[i]->getFunctions();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLMethod*> KLFile::getMethods() const
{
  std::vector<const KLMethod*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLMethod*> singleResult = m_nameSpaces[i]->getMethods();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLInterface*> KLFile::getInterfaces() const
{
  std::vector<const KLInterface*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLInterface*> singleResult = m_nameSpaces[i]->getInterfaces();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLStruct*> KLFile::getStructs() const
{
  std::vector<const KLStruct*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLStruct*> singleResult = m_nameSpaces[i]->getStructs();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLObject*> KLFile::getObjects() const
{
  std::vector<const KLObject*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLObject*> singleResult = m_nameSpaces[i]->getObjects();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLOperator*> KLFile::getOperators() const
{
  std::vector<const KLOperator*> result;
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLOperator*> singleResult = m_nameSpaces[i]->getOperators();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLError*> KLFile::getErrors() const
{
  return m_errors;
}

const KLStmt * KLFile::getStatementAtCursor(uint32_t line, uint32_t column) const
{
  uint32_t minDistance = UINT_MAX;
  const KLStmt * result = NULL;

  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    const KLStmt * statement = m_nameSpaces[i]->getStatementAtCursor(line, column);
    if(statement)
    {
      uint32_t distance = statement->getCursorDistance(line, column);
      if(distance < minDistance)
      {
        result = statement;
        minDistance = distance;
      }
      
    }
  }

  return result;
}

bool KLFile::updateKLCode(const char * code)
{
  clear();
  m_errors.clear();

  m_klCode = code;
  m_parsed = false;
  parse();

  if(!hasErrors())
    m_extension->getASTManager()->onFileParsed(this);

  return hasErrors();
}
