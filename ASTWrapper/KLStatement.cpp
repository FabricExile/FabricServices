// Copyright 2010-2014 Fabric Engine Inc. All rights reserved.

#include "KLStatement.h"
#include "KLCompoundStatement.h"
#include "KLConditionalStatement.h"
#include "KLCStyleLoopStatement.h"
#include "KLSwitchStatement.h"
#include "KLCaseStatement.h"
#include "KLVarDeclStatement.h"
#include "KLExprStatement.h"
#include "KLLocation.h"

#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLStatement::KLStatement(const KLFile* klFile, JSONData data, KLStatement * parent)
: KLCommented(klFile, data)
{
  m_type = getDictValue("type")->getStringData();
  m_parent = parent;
  if(m_parent)
    m_depth = m_parent->getDepth() + 1;
  else
    m_depth = 0;
}

KLStatement::~KLStatement()
{
  for(uint32_t i=0;i<m_statements.size();i++)
  {
    delete(m_statements[i]);
  }
}

KLDeclType KLStatement::getDeclType() const
{
  return KLDeclType_Statement;
}

bool KLStatement::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Statement)
    return true;
  return KLCommented::isOfDeclType(type);
}

std::string KLStatement::getTypeName() const
{
  return m_type;  
}

uint32_t KLStatement::getChildCount() const
{
  return m_statements.size();
}

const KLStatement * KLStatement::getChild(uint32_t index) const
{
  return m_statements[index];
}

std::vector<const KLStatement*> KLStatement::getAllChildrenOfType(KLDeclType type, bool downwards, bool upwards) const
{
  std::vector<const KLStatement*> result;
  for(size_t i=0;i<m_statements.size();i++)
  {
    if(m_statements[i]->isOfDeclType(type))
    {
      result.push_back(m_statements[i]);
    }
  } 
  if(upwards && m_parent)
  {
    std::vector<const KLStatement*> childResult = m_parent->getAllChildrenOfType(type, false, true);
    result.insert(result.end(), childResult.begin(), childResult.end());
  }
  if(downwards)
  {
    for(size_t i=0;i<m_statements.size();i++)
    {
      std::vector<const KLStatement*> childResult = m_statements[i]->getAllChildrenOfType(type, true, false);
      result.insert(result.end(), childResult.begin(), childResult.end());
    } 
  }

  return result;
}

const KLStatement * KLStatement::getStatementAtCursor(uint32_t line, uint32_t column) const
{
  uint32_t minDistance = UINT_MAX;
  const KLStatement * result = NULL;

  if(getCursorDistance(line, column) < minDistance)
  {
    result = this;
    minDistance = getCursorDistance(line, column);
  }

  for(size_t i=0;i<m_statements.size();i++)
  {
    const KLStatement * statement = m_statements[i]->getStatementAtCursor(line, column);
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

const KLStatement * KLStatement::getParent() const
{
  return m_parent;
}

const KLStatement * KLStatement::getTop() const
{
  if(!m_parent)
    return this;
  return m_parent->getTop();
}


uint32_t KLStatement::getDepth() const
{
  return m_depth;
}

const KLStatement * KLStatement::constructChild(JSONData data)
{
  std::string type = data->getDictValue("type")->getStringData();

  KLStatement * result = NULL;

  if(type == "CompoundStatement")
  {
    result = new KLCompoundStatement(getKLFile(), data, this);
  }
  else if(type == "ConditionalStatement")
  {
    result = new KLConditionalStatement(getKLFile(), data, this);
  }
  else if(type == "CStyleLoop")
  {
    result = new KLCStyleLoopStatement(getKLFile(), data, this);
  }
  else if(type == "SwitchStatement")
  {
    result = new KLSwitchStatement(getKLFile(), data, this);
  }
  else if(type == "Case")
  {
    result = new KLCaseStatement(getKLFile(), data, this);
  }
  else if(type == "VarDeclStatement")
  {
    result = new KLVarDeclStatement(getKLFile(), data, this);
  }
  else if(type == "ExprStatement")
  {
    result = new KLExprStatement(getKLFile(), data, this);
  }
  else
  {
    // printf("unresolved type '%s'\n", type.c_str());
    // printf("json '%s'\n", data->getJSONEncoding().getStringData());
    result = new KLStatement(getKLFile(), data, this);
  }

  m_statements.push_back(result);
  return result;
}

uint32_t KLStatement::getCursorDistance(uint32_t line, uint32_t column) const
{
  if(getLocation()->getLine() > line || getLocation()->getEndLine() < line)
    return UINT_MAX;
  if(getLocation()->getLine() == line && getLocation()->getColumn() > column)
    return UINT_MAX;
  if(getLocation()->getEndLine() == line && getLocation()->getEndColumn() < column)
    return UINT_MAX;

  uint32_t startDistance = 1000 * (line - getLocation()->getLine()) + column - getLocation()->getColumn();
  uint32_t endDistance = 1000 * (getLocation()->getEndLine() - line) + getLocation()->getEndColumn() - column;

  return startDistance > endDistance ? endDistance : startDistance;
}
