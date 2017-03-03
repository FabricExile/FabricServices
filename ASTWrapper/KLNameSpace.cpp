// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLNameSpace.h"
#include "KLLocation.h"
#include "KLExtension.h"

#include <FTL/Path.h>
#include <FTL/StrTrim.h>
#include <limits.h>

using namespace FabricServices::ASTWrapper;

KLNameSpace::KLNameSpace(const KLFile * klFile, const KLNameSpace * nameSpace, JSONData data)
: KLCommented(klFile, nameSpace, data)
{
  JSONData nameData = data->getDictValue("namespacePath");
  if(nameData)
    m_name = nameData->getStringData();
}

KLNameSpace::~KLNameSpace()
{
  clear();
}

KLDeclType KLNameSpace::getDeclType() const
{
  return KLDeclType_NameSpace;
}

bool KLNameSpace::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_NameSpace)
    return true;
  return KLCommented::isOfDeclType(type);
}

const char * KLNameSpace::getName() const
{
  return m_name.c_str();  
}

void KLNameSpace::clear()
{
  for(uint32_t i=0;i<m_requires.size();i++)
    delete(m_requires[i]);
  for(uint32_t i=0;i<m_nameSpaces.size();i++)
    delete(m_nameSpaces[i]);
  for(uint32_t i=0;i<m_aliases.size();i++)
    delete(m_aliases[i]);
  for(uint32_t i=0;i<m_constants.size();i++)
    delete(m_constants[i]);
  for(uint32_t i=0;i<m_types.size();i++)
    delete(m_types[i]);
  for(uint32_t i=0;i<m_functions.size();i++)
    delete(m_functions[i]);
  for(uint32_t i=0;i<m_operators.size();i++)
    delete(m_operators[i]);
  m_requires.clear();
  m_nameSpaces.clear();
  m_aliases.clear();
  m_constants.clear();
  m_types.clear();
  m_functions.clear();
  m_operators.clear();
}

void KLNameSpace::parseJSON( FabricCore::Variant const *astVariant )
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

      // printf("KLNameSpace: %s\n", et.c_str());

      if(et == "RequireGlobal")
      {
        KLRequire * e = new KLRequire(getKLFile(), this, element);
        m_requires.push_back(e);

        // ensure to parse extensions in the right order,
        // so that we can add methods to types for example.
        KLExtension * extension = (KLExtension *)getExtension()->getASTManager()->getExtension(e);

        if(extension == NULL && getExtension()->getASTManager()->getAutoLoadExtensions())
        {
          const char * extName = e->getRequiredExtension().c_str();
          KLASTManager * manager = ((KLASTManager*)getExtension()->getASTManager());
          extension = (KLExtension *)manager->loadExtensionFromExtsPath(extName);
        }

        if(extension)
          extension->parse();
      }
      else if ( et == "ASTNamespaceGlobal" )
      {
        KLNameSpace * e = new KLNameSpace(getKLFile(), this, element);
        m_nameSpaces.push_back(e);
        e->parseJSON( element->getDictValue( "globalList" ) );
      }
      else if ( et == "ASTUsingGlobal" )
      {
        // do nothing
      }
      else if(et == "Alias")
      {
        KLAlias * e = new KLAlias(getKLFile(), this, element);
        m_aliases.push_back(e);
      }
      else if(et == "GlobalConstDecl")
      {
        KLConstant * e = new KLConstant(getKLFile(), this, element);
        m_constants.push_back(e);
      }
      else if(et == "Function")
      {
        KLFunction * e = new KLFunction(getKLFile(), this, element);
        const KLType * klType = getExtension()->getASTManager()->getKLTypeByName(e->getName().c_str(), e);
        if(klType)
        {
          KLMethod * m = new KLMethod(getKLFile(), this, element, e->getName());
          if(!klType->pushMethod(m))
            m_functions.push_back(m);
          else
            m_methods.push_back(m);
          delete(e);
        }
        else
        {
          m_functions.push_back(e);
        }
      }
      else if(et == "Operator")
      {
        KLOperator * e = new KLOperator(getKLFile(), this, element);
        m_operators.push_back(e);
      }
      else if(et == "ASTStructDecl")
      {
        KLStruct * e = new KLStruct(getKLFile(), this, element);
        if(e->isForwardDecl())
        {
          getKLFile()->getExtensionMutable()->storeForwardDeclComments(e);
          delete(e);
        }
        else
        {
          getKLFile()->getExtensionMutable()->consumeForwardDeclComments(e);
          m_types.push_back(e);
        }
      }
      else if(et == "MethodOpImpl")
      {
        KLMethod * e = new KLMethod(getKLFile(), this, element);
        std::string thisType = e->getThisType();

        const KLType * klType = getExtension()->getASTManager()->getKLTypeByName(thisType.c_str(), e);
        if(klType)
        {
          if(!klType->pushMethod(e))
            m_functions.push_back(e);
          else
            m_methods.push_back(e);
        }
        else
        {
          m_functions.push_back(e);
        }
      }
      else if(et == "Destructor")
      {
        KLFunction function(getKLFile(), this, element);
        std::string thisType = function.getName();
        FTL::StrTrimLeft<'~'>( thisType );
        KLMethod * e = new KLMethod(getKLFile(), this, element, thisType);
        const KLType * klType = getExtension()->getASTManager()->getKLTypeByName(thisType.c_str(), e);
        if(klType)
        {
          if(!klType->pushMethod(e))
            m_functions.push_back(e);
        }
        else
          m_functions.push_back(e);
      }
      else if(et == "ASTInterfaceDecl")
      {
        KLInterface * e = new KLInterface(getKLFile(), this, element);
        if(e->isForwardDecl())
        {
          getKLFile()->getExtensionMutable()->storeForwardDeclComments(e);
          delete(e);
        }
        else
        {
          getKLFile()->getExtensionMutable()->consumeForwardDeclComments(e);
          m_types.push_back(e);
        }
      }
      else if(et == "ASTObjectDecl")
      {
        KLObject * e = new KLObject(getKLFile(), this, element);
        if(e->isForwardDecl())
        {
          getKLFile()->getExtensionMutable()->storeForwardDeclComments(e);
          delete(e);
        }
        else
        {
          getKLFile()->getExtensionMutable()->consumeForwardDeclComments(e);
          m_types.push_back(e);
        }
      }
      else if(et == "ComparisonOpImpl" ||
        et == "AssignOpImpl" ||
        et == "BinOpImpl" ||
        et == "ASTUniOpDecl")
      {
        KLTypeOp * e = new KLTypeOp(getKLFile(), this, element);

        std::string thisType = e->getLhs();
        const KLType * klType = getExtension()->getASTManager()->getKLTypeByName(thisType.c_str(), e);
        if(klType)
          klType->pushTypeOp(e);
        else
          m_functions.push_back(e);
      }
      else
      {
        std::string message = "KLNameSpace: Unknown AST token '"+et+"'.";
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

std::vector<const KLRequire*> KLNameSpace::getRequires() const
{
  std::vector<const KLRequire*> result;
  result.insert(result.end(), m_requires.begin(), m_requires.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLRequire*> singleResult = m_nameSpaces[i]->getRequires();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLNameSpace*> KLNameSpace::getNameSpaces() const
{
  std::vector<const KLNameSpace*> result;
  result.insert(result.end(), m_nameSpaces.begin(), m_nameSpaces.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLNameSpace*> singleResult = m_nameSpaces[i]->getNameSpaces();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLAlias*> KLNameSpace::getAliases() const
{
  std::vector<const KLAlias*> result;
  result.insert(result.end(), m_aliases.begin(), m_aliases.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLAlias*> singleResult = m_nameSpaces[i]->getAliases();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLConstant*> KLNameSpace::getConstants() const
{
  std::vector<const KLConstant*> result;
  result.insert(result.end(), m_constants.begin(), m_constants.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLConstant*> singleResult = m_nameSpaces[i]->getConstants();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLType*> KLNameSpace::getTypes() const
{
  std::vector<const KLType*> result;
  result.insert(result.end(), m_types.begin(), m_types.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLType*> singleResult = m_nameSpaces[i]->getTypes();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLFunction*> KLNameSpace::getFunctions() const
{
  std::vector<const KLFunction*> result;
  result.insert(result.end(), m_functions.begin(), m_functions.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLFunction*> singleResult = m_nameSpaces[i]->getFunctions();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLMethod*> KLNameSpace::getMethods() const
{
  std::vector<const KLMethod*> result;
  result.insert(result.end(), m_methods.begin(), m_methods.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLMethod*> singleResult = m_nameSpaces[i]->getMethods();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLOperator*> KLNameSpace::getOperators() const
{
  std::vector<const KLOperator*> result;
  result.insert(result.end(), m_operators.begin(), m_operators.end());
  for(size_t i=0;i<m_nameSpaces.size();i++)
  {
    std::vector<const KLOperator*> singleResult = m_nameSpaces[i]->getOperators();
    result.insert(result.end(), singleResult.begin(), singleResult.end());
  }
  return result;
}

std::vector<const KLInterface*> KLNameSpace::getInterfaces() const
{
  std::vector<const KLType*> allTypes = getTypes();
  std::vector<const KLInterface*> result;
  for(uint32_t i=0;i<allTypes.size();i++)
  {
    if(allTypes[i]->getKLType() == std::string("interface"))
      result.push_back((const KLInterface*)allTypes[i]);
  }
  return result;
}

std::vector<const KLStruct*> KLNameSpace::getStructs() const
{
  std::vector<const KLType*> allTypes = getTypes();
  std::vector<const KLStruct*> result;
  for(uint32_t i=0;i<allTypes.size();i++)
  {
    if(allTypes[i]->getKLType() == std::string("struct"))
      result.push_back((const KLStruct*)allTypes[i]);
  }
  return result;
}

std::vector<const KLObject*> KLNameSpace::getObjects() const
{
  std::vector<const KLType*> allTypes = getTypes();
  std::vector<const KLObject*> result;
  for(uint32_t i=0;i<allTypes.size();i++)
  {
    if(allTypes[i]->getKLType() == std::string("object"))
      result.push_back((const KLObject*)allTypes[i]);
  }
  return result;
}

const KLStmt * KLNameSpace::getStatementAtCursor(uint32_t line, uint32_t column) const
{
  uint32_t minDistance = UINT_MAX;
  const KLStmt * result = NULL;

  for(size_t i=0;i<m_functions.size();i++)
  {
    const KLStmt * statement = m_functions[i]->getStatementAtCursor(line, column);
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

  for(size_t i=0;i<m_operators.size();i++)
  {
    const KLStmt * statement = m_operators[i]->getStatementAtCursor(line, column);
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

  for(size_t i=0;i<m_types.size();i++)
  {
    for(uint32_t j=0;j<m_types[i]->getMethodCount();j++)
    {
      const KLStmt * statement = m_types[i]->getMethod(j)->getStatementAtCursor(line, column);
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

    for(uint32_t j=0;j<m_types[i]->getTypeOpCount();j++)
    {
      const KLStmt * statement = m_types[i]->getTypeOp(j)->getStatementAtCursor(line, column);
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
  }

  return result;
}
