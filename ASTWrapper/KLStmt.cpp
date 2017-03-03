// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLStmt.h"
#include "KLCompoundStmt.h"
#include "KLConditionalStmt.h"
#include "KLCStyleLoopStmt.h"
#include "KLSwitchStmt.h"
#include "KLCaseStmt.h"
#include "KLVarDeclStmt.h"
#include "KLExprStmt.h"
#include "KLLocation.h"

#include <limits.h>
#include <vector>
#include <string>

using namespace FabricServices::ASTWrapper;

KLStmt::KLStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent)
: KLCommented(klFile, nameSpace, data)
{
  m_type = getDictValue("type")->getStringData();
  m_parent = parent;
  if(m_parent)
    m_depth = m_parent->getDepth() + 1;
  else
    m_depth = 0;
}

KLStmt::~KLStmt()
{
  for(uint32_t i=0;i<m_statements.size();i++)
  {
    delete(m_statements[i]);
  }
}

KLDeclType KLStmt::getDeclType() const
{
  return KLDeclType_Stmt;
}

bool KLStmt::isOfDeclType(KLDeclType type) const
{
  if(type == KLDeclType_Stmt)
    return true;
  return KLCommented::isOfDeclType(type);
}

std::string KLStmt::getTypeName() const
{
  return m_type;  
}

uint32_t KLStmt::getChildCount() const
{
  return m_statements.size();
}

const KLStmt * KLStmt::getChild(uint32_t index) const
{
  return m_statements[index];
}

std::vector<const KLStmt*> KLStmt::getAllChildrenOfType(KLDeclType type, bool downwards, bool upwards) const
{
  std::vector<const KLStmt*> result;
  for(size_t i=0;i<m_statements.size();i++)
  {
    if(m_statements[i]->isOfDeclType(type))
    {
      result.push_back(m_statements[i]);
    }
  } 
  if(upwards && m_parent)
  {
    std::vector<const KLStmt*> childResult = m_parent->getAllChildrenOfType(type, false, true);
    result.insert(result.end(), childResult.begin(), childResult.end());
  }
  if(downwards)
  {
    for(size_t i=0;i<m_statements.size();i++)
    {
      std::vector<const KLStmt*> childResult = m_statements[i]->getAllChildrenOfType(type, true, false);
      result.insert(result.end(), childResult.begin(), childResult.end());
    } 
  }

  return result;
}

const KLStmt * KLStmt::getStatementAtCursor(uint32_t line, uint32_t column) const
{
  uint32_t minDistance = UINT_MAX;
  const KLStmt * result = NULL;

  if(getCursorDistance(line, column) < minDistance)
  {
    result = this;
    minDistance = getCursorDistance(line, column);
  }

  for(size_t i=0;i<m_statements.size();i++)
  {
    const KLStmt * statement = m_statements[i]->getStatementAtCursor(line, column);
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

const KLStmt * KLStmt::getParent() const
{
  return m_parent;
}

const KLStmt * KLStmt::getTop() const
{
  if(!m_parent)
    return this;
  return m_parent->getTop();
}


uint32_t KLStmt::getDepth() const
{
  return m_depth;
}

const KLStmt * KLStmt::constructChild(JSONData data)
{
  std::string type = data->getDictValue("type")->getStringData();

  KLStmt * result = NULL;

  if(type == "CompoundStatement")
  {
    result = new KLCompoundStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "ASTCondStmt")
  {
    result = new KLConditionalStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "CStyleLoop")
  {
    result = new KLCStyleLoopStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "SwitchStatement")
  {
    result = new KLSwitchStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "Case")
  {
    result = new KLCaseStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "VarDeclStatement")
  {
    result = new KLVarDeclStmt(getKLFile(), getNameSpace(), data, this);
  }
  else if(type == "ExprStatement")
  {
    result = new KLExprStmt(getKLFile(), getNameSpace(), data, this);
  }
  else
  {
    // printf("unresolved type '%s'\n", type.c_str());
    // printf("json '%s'\n", data->getJSONEncoding().getStringData());
    result = new KLStmt(getKLFile(), getNameSpace(), data, this);
  }

  m_statements.push_back(result);
  return result;
}

uint32_t KLStmt::getCursorDistance(uint32_t line, uint32_t column) const
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
