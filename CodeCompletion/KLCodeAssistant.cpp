// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLCodeAssistant.h"
#include <FTL/StrFilter.h>
#include <FTL/StrSplit.h>

using namespace FabricServices::ASTWrapper;
using namespace FabricServices::CodeCompletion;

struct MatchCharBackSlashR
{
  MatchCharBackSlashR() {}
  bool operator()( char ch ) const
  {
    return ch == '\r';
  }
};

struct MatchCharBraces
{
  MatchCharBraces() {}
  bool operator()( char ch ) const
  {
    return ch == '(' || ch == ')';
  }
};

KLCodeAssistant::KLCodeAssistant(KLASTManager * manager)
: ASTWrapper::KLASTClient(manager)
{
  m_highlighter = new KLSyntaxHighlighter(manager);
  m_owningHighlighter = true;
  init();
}

KLCodeAssistant::KLCodeAssistant(KLSyntaxHighlighter * highlighter)
: ASTWrapper::KLASTClient(highlighter->getASTManager())
{
  m_highlighter = highlighter;
  m_owningHighlighter = false;
  init();
}

KLCodeAssistant::~KLCodeAssistant()
{
}

void KLCodeAssistant::init()
{
  m_file = NULL;
}

bool KLCodeAssistant::setASTManager(KLASTManager * manager)
{
  if(ASTWrapper::KLASTClient::setASTManager(manager))
  {
    if(m_owningHighlighter && m_highlighter)
      m_highlighter->setASTManager(manager);
    return true;
  }
  return false;
}

KLSyntaxHighlighter * KLCodeAssistant::getHighlighter()
{
  return m_highlighter;
}

const KLFile * KLCodeAssistant::getKLFile()
{
  return m_file;
}

std::vector<const KLError*> KLCodeAssistant::getKLErrors()
{
  if(m_file)
    m_file->getErrors();
  return std::vector<const KLError*>();
}

bool KLCodeAssistant::updateCurrentKLFile(const KLFile * file)
{
  if(!hasASTManager())
    return false;
  if(m_file == file)
    return false;
  m_file = file;
  if(!m_file)
    return false;

  m_fileName = m_file->getAbsoluteFilePath();
  m_code = m_file->getKLCode();

  m_code = FTL::StrFilter<MatchCharBackSlashR>( m_code );
  m_lines.clear();
  FTL::StrSplit<'\n'>(m_code, m_lines, true);
  return true;
}

bool KLCodeAssistant::updateCurrentCodeAndFile(const std::string & code, const std::string & fileName, bool updateAST, FabricCore::DFGExec *dfgExec )
{
  if(!hasASTManager())
    return false;

  if(code.length() == 0 || fileName.length() == 0)
    return false;

  std::string newCode = code;
  newCode = FTL::StrFilter<MatchCharBackSlashR>( newCode );
  if(m_code == newCode && m_fileName == fileName)
    return false;

  m_code = newCode;
  m_fileName = fileName;
  
  m_lines.clear();
  FTL::StrSplit<'\n'>(m_code, m_lines, true);

  if(updateAST)
  {
    if(m_file == NULL)
      m_file = getASTManager()->loadSingleKLFile(m_fileName.c_str(), m_code.c_str(), dfgExec);
    else if(m_file->getAbsoluteFilePath() != fileName)
      m_file = getASTManager()->loadSingleKLFile(m_fileName.c_str(), m_code.c_str(), dfgExec);
    else
      ((KLFile*)m_file)->updateKLCode(m_code.c_str());

    // update all error formats
    m_highlighter->clearErrors();
    if(m_file->hasErrors())
    {
      std::vector<const KLError*> errors = m_file->getErrors();
      for(size_t i=0;i<errors.size();i++)
      {
        if(errors[i]->getLine() <= static_cast<int>( m_lines.size() ))
        {
          uint32_t cursor;
          lineAndColumnToCursor(errors[i]->getLine(), 1, cursor);
          m_highlighter->reportError(cursor, m_lines[errors[i]->getLine()-1].length());
        }
      }
    }
  }

  return updateAST;
}

void KLCodeAssistant::lineAndColumnToCursor(uint32_t line, uint32_t column, uint32_t & cursor) const
{
  cursor = 0;
  for(uint32_t l=1; l<line; l++)
  {
    if(l >= m_lines.size())
      return;
    cursor += m_lines[l-1].length() + 1;
  }
  cursor += column - 1;
}

void KLCodeAssistant::cursorToLineAndColumn(uint32_t cursor,  uint32_t & line, uint32_t & column) const
{
  line = 0;
  column = 0;
  if(m_lines.size() == 0)
    return;

  uint32_t sum = cursor;
  for(line=1; line<=m_lines.size(); line++)
  {
    if(sum <= m_lines[line-1].length())
    {
      column = sum + 1;
      return;
    }
    sum -= m_lines[line-1].length() + 1;
  }

  column = m_lines[m_lines.size() - 1].length() + 1;
}

bool KLCodeAssistant::isCursorInsideCommentOrString(uint32_t cursor) const
{
  if(!hasASTManager())
    return false;
  if(cursor >= m_code.length())
    return false;

  const std::vector<KLSyntaxHighlighter::Format> & formats = m_highlighter->getHighlightFormats(m_code);
  for(size_t i=0;i<formats.size();i++)
  {
    if(formats[i].token == KLSyntaxHighlighter::Token(FEC_KLTokenType_Comment)
      || formats[i].token == KLSyntaxHighlighter::Token(FEC_KLTokenType_String))
    {
      if(formats[i].start <= cursor && formats[i].start + formats[i].length >= cursor)
        return true;
    }
  }

  return false;
}

bool KLCodeAssistant::isCursorInsideCommentOrString(uint32_t line, uint32_t column) const
{
  uint32_t cursor = 0;
  lineAndColumnToCursor(line, column, cursor);
  return isCursorInsideCommentOrString(cursor);
}

const KLStmt * KLCodeAssistant::getStatementAtCursor(uint32_t cursor) const
{
  uint32_t line = 0;
  uint32_t column = 0;
  cursorToLineAndColumn(cursor, line, column);
  return getStatementAtCursor(line, column);
}

const KLStmt * KLCodeAssistant::getStatementAtCursor(uint32_t line, uint32_t column) const
{
  if(!hasASTManager())
    return NULL;
  if(!m_file)
    return NULL;
  return m_file->getStatementAtCursor(line, column);
}

std::string KLCodeAssistant::getCodeForStatement(const KLStmt * statement) const
{
  uint32_t start, end;
  lineAndColumnToCursor(statement->getLocation()->getLine(), statement->getLocation()->getColumn(), start);
  lineAndColumnToCursor(statement->getLocation()->getEndLine(), statement->getLocation()->getEndColumn(), end);
  return m_code.substr(start, end - start);
}

std::string KLCodeAssistant::getWordAtCursor(uint32_t cursor, bool ignoreParentheses) const
{
  uint32_t line, column;
  cursorToLineAndColumn(cursor, line, column);
  return getWordAtCursor(line, column, ignoreParentheses);
}

#define isWordChar(c) (isalnum(c) || (c == '_') || (ignoreParentheses && (c == '(' || c == ')')))

std::string KLCodeAssistant::getWordAtCursor(uint32_t line, uint32_t column, bool ignoreParentheses) const
{
  if(line > m_lines.size() || line == 0)
    return "";

  const std::string & l = m_lines[line-1];
  if(column > l.length())
    column = l.length();
  if(column == 0)
    return "";

  uint32_t s = column - 1;
  uint32_t e = column - 1;
  while(s > 0)
  {
    if(!isWordChar(l[s-1]))
      break;
    s--;
  }

  while(e < l.length()-1)
  {
    if(!isWordChar(l[e+1]))
      break;
    e++;
  }

  if(isWordChar(l[s]) && isWordChar(l[e]))
  {
    std::string result = l.substr(s, e - s + 1);
    result = FTL::StrFilter<MatchCharBraces>( result );
    return result;
  }

  return "";
}

std::string KLCodeAssistant::getCharAtCursor(uint32_t cursor) const
{
  uint32_t line, column;
  cursorToLineAndColumn(cursor, line, column);
  return getCharAtCursor(line, column);
}

std::string KLCodeAssistant::getCharAtCursor(uint32_t line, uint32_t column) const
{
  if(line > m_lines.size() || line == 0)
    return "";

  const std::string & l = m_lines[line-1];
  if(column > l.length())
    column = l.length();
  if(column == 0)
    return "";

  return l.substr(column-1, 1);
}

std::vector<KLVariable> KLCodeAssistant::getVariablesAtCursor(uint32_t cursor) const
{
  uint32_t line, column;
  cursorToLineAndColumn(cursor, line, column);
  return getVariablesAtCursor(line, column);
}

std::vector<KLVariable> KLCodeAssistant::getVariablesAtCursor(uint32_t line, uint32_t column) const
{
  std::vector<KLVariable> result;
  if(!hasASTManager())
    return result;
  const KLStmt * statement = getStatementAtCursor(line, column);
  if(statement)
  {
    if(statement->isOfDeclType(KLDeclType_Function))
      statement = ((KLFunction*)statement)->getBody();
  }
  if(statement)
  {
    std::vector<const KLStmt *> varDecls = statement->getAllChildrenOfType(KLDeclType_VarDeclStmt, false, true);
    for(size_t i=0;i<varDecls.size();i++)
    {
      const KLVarDeclStmt * varDecl = (const KLVarDeclStmt *)varDecls[i];
      for(uint32_t j=0;j<varDecl->getCount();j++)
      {
        result.push_back(KLVariable(varDecl->getName(j), varDecl->getBaseType(), varDecl->getArrayModifier(j)));
      }
    }

    statement = statement->getTop();

    if(statement->isOfDeclType(KLDeclType_Function))
    {
      KLFunction * function = (KLFunction *)statement;
      for(uint32_t i=0;i<function->getParameterCount();i++)
      {
        const KLParameter * param = function->getParameter(i);
        result.push_back(KLVariable(param->getName(), param->getType()));
      }        
    }
  }

  return result;
}

const KLDecl * KLCodeAssistant::getDeclAtCursor(uint32_t cursor) const
{
  uint32_t line, column;
  cursorToLineAndColumn(cursor, line, column);
  return getDeclAtCursor(line, column);
}

const KLDecl * KLCodeAssistant::getDeclAtCursor(uint32_t line, uint32_t column) const
{
  if(!hasASTManager())
    return NULL;
  if(line > m_lines.size() || line == 0)
    return NULL;

  const std::string & l = m_lines[line-1];
  if(column > l.length())
    column = l.length();
  if(column == 0)
    return NULL;

  std::vector<std::string> delegates;
  uint32_t braces = 0;
  uint32_t brackets = 0;
  uint32_t singleQuotes = 0;
  uint32_t doubleQuotes = 0;

  int i = column - 1;

  bool resolved = false;
  std::string word;
  while(i >= 0 && !resolved)
  {
    char c = l[i];
    if(c == ')')
    {
      braces++;
    }    
    else if(c == '(')
    {
      if(braces > 0)
        braces--;
    }    
    else if(c == ']')
    {
      brackets++;
    }    
    else if(c == '[')
    {
      if(brackets > 0)
        brackets--;
    }    
    else if(c == '\'' && doubleQuotes == 0)
    {
      singleQuotes = singleQuotes == 1 ? 0 : 1;
    }    
    else if(c == '\'' && singleQuotes == 0)
    {
      doubleQuotes = doubleQuotes == 1 ? 0 : 1;
    }    
    else if(braces == 0 && brackets == 0 && singleQuotes == 0 && doubleQuotes == 0 && (isalnum(c) || c == '_'))
    {
      word = c + word;
    }
    else if(c == '.')
    {
      if(word.length() > 0)
      {
        if(delegates.size() == 0)
        {
          size_t i2 = column;
          while(i2 < l.length())
          {
            c = l[i2++];
            if(isalnum(c) || c == '_')
              word = word + c;
            else
              break;
          }
        }
        delegates.insert(delegates.begin(), word);
        word = "";
      }
    }
    else if(c == ' ' || c == '\t')
    {
      if(delegates.size() == 0 && word.length() > 0)
      {
        size_t i2 = column;
        while(i2 < l.length())
        {
          c = l[i2++];
          if(isalnum(c) || c == '_')
            word = word + c;
          else
            break;
        }
        resolved = true;
      }
    }
    else if(braces == 0 && brackets == 0 && singleQuotes == 0 && doubleQuotes == 0)
    {
      resolved = true;
    }
    i--;
  }

  const KLType * type = NULL;
  if(word.length() > 0)
  {
    // try all variables
    if(type == NULL)
    {
      std::vector<KLVariable> vars = getVariablesAtCursor(line, i+1);
      for(size_t i=0;i<vars.size();i++)
      {
        if(vars[i].getName() == word)
        {
          type = getASTManager()->getKLTypeByName(resolveAliases(vars[i].getBaseType().c_str()), m_file);
          if(type)
            break;
        }
      }
    }

    // maybe this is a type itself
    if(type == NULL)
    {
      type = getASTManager()->getKLTypeByName(resolveAliases(word.c_str()), m_file);
    }

    // maybe this is a function
    if(type == NULL)
    {
      std::vector<const KLFunction*> functions = getASTManager()->getFunctions();
      for(size_t i=0;i<functions.size();i++)
      {
        if(functions[i]->getName() == word)
        {
          if(delegates.size() == 0)
            return functions[i];

          KLTypeDesc returnType(functions[i]->getReturnType());
          type = getASTManager()->getKLTypeByName(resolveAliases(returnType.getBaseType().c_str()), m_file);
          if(type)
            break;
        }
      }
    }

    // maybe this is a constant
    if(type == NULL)
    {
      std::vector<const KLConstant*> constants = getASTManager()->getConstants();
      for(size_t i=0;i<constants.size();i++)
      {
        if(constants[i]->getName() == word)
        {
          return constants[i];
        }
      }
    }
  }

  if(type)
  {
    for(size_t i=0;i<delegates.size();i++)
    {
      bool found = false;
      std::vector<const KLMethod*> methods = type->getMethods(true);
      for(size_t j=0;j<methods.size();j++)
      {
        const KLMethod * method = methods[j];
        if(method == NULL)
          continue;
        if(method->getName() != delegates[i])
          continue;

        if(i == delegates.size()-1)
          return method;

        KLTypeDesc returnType(method->getReturnType());
        type = getASTManager()->getKLTypeByName(resolveAliases(returnType.getBaseType().c_str()), m_file);
        if(type == NULL)
          return NULL;
        found = true;
        break;
      }

      if(!found)
      {
        if(type->getDeclType() != KLDeclType_Interface)
        {
          const KLStruct * castType = (const KLStruct *)type;
          const KLMember * member = castType->getMember(delegates[i].c_str(), true);
          if(member)
          {
            KLTypeDesc memberType(member->getType());
            type = getASTManager()->getKLTypeByName(memberType.getBaseType().c_str(), m_file);
            if(type == NULL)
              return NULL;
          }
        }
      }
    }
  }

  return type;
}

const KLType * KLCodeAssistant::getTypeAtCursor(uint32_t cursor, bool ignoreInvalidMethod) const
{
  uint32_t line, column;
  cursorToLineAndColumn(cursor, line, column);
  return getTypeAtCursor(line, column, ignoreInvalidMethod);
}

const KLType * KLCodeAssistant::getTypeAtCursor(uint32_t line, uint32_t column, bool ignoreInvalidMethod) const
{
  const KLDecl * decl = getDeclAtCursor(line, column);
  if(decl)
  {
    if(decl->isOfDeclType(KLDeclType_Type))
    {
      return (const KLType *)decl;
    }
    else if(decl->isOfDeclType(KLDeclType_Function))
    {
      KLTypeDesc typeDesc(((const KLFunction*)decl)->getReturnType());
      if(ignoreInvalidMethod)
      {
        if(decl->isOfDeclType(KLDeclType_Method))
        {
          // walk backwards and check if there is a brace
          std::string l = m_lines[line-1];
          int c = column-1;
          while(c >= 0)
          {
            if(l[c] == ')')
              break;
            else if(isalnum(l[c]) || l[c] == '_')
            {
              typeDesc = KLTypeDesc(((const KLMethod*)decl)->getThisType());
              break;
            }
            c--;
          }
        }
      }
      return getASTManager()->getKLTypeByName(resolveAliases(typeDesc.getBaseType().c_str()), m_file);
    }
  }
  return NULL;
}

const char * KLCodeAssistant::resolveAliases(const char * name) const
{
  if(!hasASTManager())
    return name;
  std::vector<const KLAlias*> aliases = getASTManager()->getAliases();
  const char * result = name;

  bool found = true;
  while(found)
  {
    found = false;
    for(size_t i=0;i<aliases.size();i++)
    {
      if(aliases[i]->getNewUserName() == result)
      {
        result = aliases[i]->getOldUserName().c_str();
        found = true;
        break;
      }
    }
  }
  
  return result;
}


