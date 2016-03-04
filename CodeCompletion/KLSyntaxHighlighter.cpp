// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLSyntaxHighlighter.h"

using namespace FabricServices;
using namespace FabricServices::CodeCompletion;

KLSyntaxHighlighter::KLSyntaxHighlighter(ASTWrapper::KLASTManager * manager)
: KLASTClient(manager)
{
  m_enabled = true;
  m_basicTypesInitialized = false;

  m_knownTokens.insert(std::pair<std::string, Token>("dfgEntry", Token_Keyword));
  m_knownTokens.insert(std::pair<std::string, Token>("report", Token_Function));
}

KLSyntaxHighlighter::~KLSyntaxHighlighter()
{
}

bool KLSyntaxHighlighter::isEnabled() const
{
  return m_enabled;
}

void KLSyntaxHighlighter::setEnabled(bool state)
{
  m_enabled = state;
}

const char * KLSyntaxHighlighter::getTokenName(Token token)
{
  switch ( token )
  {
    case Token_Comment: return "Comment";
    case Token_String: return "String";
    case Token_Number: return "Number";
    case Token_Keyword: return "Keyword";
    case Token_Other: return "Other";
    case Token_Error: return "Error";
    case Token_EOF: return "EOF";
    case Token_Type: return "Type";
    case Token_Constant: return "Constant";
    case Token_Function: return "Function";
    case Token_Method: return "Method";
    case Token_Highlight: return "Highlight";
    default: return "***UNKNOWN***";
  }
}

const std::vector<KLSyntaxHighlighter::Format> & KLSyntaxHighlighter::getHighlightFormats(const std::string & text) const
{
  if(!m_enabled || !hasASTManager())
  {
    m_lastFormats.clear();
    m_lastText = "";
    return m_lastFormats;
  }

  if(m_lastText == text)
    return m_lastFormats;

  std::vector<Format> formats;

  FabricCore::KLTokenStream klTokenStream = FabricCore::KLTokenStream::Create(*getASTManager()->getClient(), text.c_str(), text.length());
  for (;;)
  {
    Format f;
    f.token = (Token)(int)klTokenStream.getNext( &f.start, &f.length );
    f.length = f.length - f.start;
    
    if(f.token == (Token)FEC_KLTokenType_EOF)
      break;

    if(f.token == Token_Other)
    {
      if(text[f.start] == ' ') continue;
      if(text[f.start] == '\n') continue;
      if(text[f.start] == '\r') continue;
      if(text[f.start] == '\t') continue;
      if(text[f.start] == '.') continue;
      if(text[f.start] == ',') continue;
      if(text[f.start] == '(') continue;
      if(text[f.start] == ')') continue;
      if(text[f.start] == '{') continue;
      if(text[f.start] == '}') continue;
      if(text[f.start] == '[') continue;
      if(text[f.start] == ']') continue;
      if(text[f.start] == '+') continue;
      if(text[f.start] == '-') continue;
      if(text[f.start] == '*') continue;
      if(text[f.start] == '/') continue;
      if(text[f.start] == ':') continue;
      if(text[f.start] == ';') continue;

      std::map<std::string, Token>::iterator it = m_knownTokens.find(text.substr(f.start, f.length));
      if(it == m_knownTokens.end() && f.start == 0)
        continue;
      if(it == m_knownTokens.end())
      {
        if(text[f.start-1] == '.')
          f.token = Token_Method;
        else
          continue;
      }
      else
        f.token = it->second;
    }

    formats.push_back(f);
  }

  std::map<uint32_t, size_t> sortedIndices;
  for(size_t i=0;i<formats.size();i++)
  {
    std::map<uint32_t, size_t>::iterator it = sortedIndices.find(formats[i].start);
    if(it == sortedIndices.end())
      sortedIndices.insert(std::pair<uint32_t, size_t>(formats[i].start, i));
  }

  m_lastText = text;
  m_lastFormats.clear();
  uint32_t end = 0;
  for(std::map<uint32_t, size_t>::iterator it = sortedIndices.begin(); it != sortedIndices.end(); it++)
  {
    // ensure to avoid overlapping formats
    Format f = formats[it->second];
    if(f.start < end)
      continue;
    m_lastFormats.push_back(f);
    end = f.start + f.length - 1;
  }

  m_lastFormats.insert(m_lastFormats.end(), m_errorFormats.begin(), m_errorFormats.end());
  m_lastFormats.insert(m_lastFormats.end(), m_highlightFormats.begin(), m_highlightFormats.end());

  return m_lastFormats;
}

std::string KLSyntaxHighlighter::getHighlightedText(const std::string & text) const
{
  const std::vector<Format> & formats = getHighlightFormats(text);
  if(formats.size() == 0)
    return text;

  std::string result;
  uint32_t end = 0;
  for(size_t i=0;i<formats.size();i++)
  {
    if(formats[i].start > end)
      result += text.substr(end, formats[i].start - end);
    // result += formats[i].prefix;
    result += text.substr(formats[i].start, formats[i].length);
    // result += formats[i].suffix;
    end = formats[i].start + formats[i].length;
  }

  if(end < text.length())
    result += text.substr(end, text.length() - end);

  return result;
}

void KLSyntaxHighlighter::reportError(uint32_t start, uint32_t length)
{
  Format f;
  f.token = Token_Error;
  f.start = start;
  f.length = length;
  m_errorFormats.push_back(f);
  m_lastText = "";
}

void KLSyntaxHighlighter::clearErrors()
{
  m_errorFormats.clear();
  m_lastText = "";
}

void KLSyntaxHighlighter::highlight(uint32_t start, uint32_t length)
{
  Format f;
  f.token = Token_Highlight;
  f.start = start;
  f.length = length;
  m_highlightFormats.push_back(f);
  m_lastText = "";
}

void KLSyntaxHighlighter::clearHighlighting()
{
  m_highlightFormats.clear();
  m_lastText = "";
}

void KLSyntaxHighlighter::onFileParsed(const ASTWrapper::KLFile * file)
{
  if(!m_basicTypesInitialized)
  {
    // ask the ast manager for all basic types
    const FabricCore::Client * client = getASTManager()->getClient();
    FabricCore::Variant registeredTypes = FabricCore::GetRegisteredTypes_Variant(*client);
    for(FabricCore::Variant::DictIter keyIter(registeredTypes); !keyIter.isDone(); keyIter.next())
    {
      std::string key = keyIter.getKey()->getStringData();
      if(key.substr(key.length()-1, 1) == ">")
        continue;
      if(GetRegisteredTypeIsStruct(*client, key.c_str()))
        continue;
      if(GetRegisteredTypeIsObject(*client, key.c_str()))
        continue;
      if(GetRegisteredTypeIsInterface(*client, key.c_str()))
        continue;
      if(m_knownTokens.find(key) != m_knownTokens.end())
        continue;
      m_knownTokens.insert(std::pair<std::string, Token>(key, Token_Type));
    }

    m_basicTypesInitialized = true;
  }

  std::vector<const ASTWrapper::KLConstant*> constants = file->getConstants();
  std::vector<const ASTWrapper::KLType*> types = file->getTypes();
  std::vector<const ASTWrapper::KLAlias*> aliases = file->getAliases();
  std::vector<const ASTWrapper::KLFunction*> functions = file->getFunctions();

  for(size_t i=0;i<constants.size();i++)
  {
    if(m_knownTokens.find(constants[i]->getName()) != m_knownTokens.end())
      continue;
    m_knownTokens.insert(std::pair<std::string, Token>(constants[i]->getName(), Token_Constant));
  }

  for(size_t i=0;i<types.size();i++)
  {
    if(m_knownTokens.find(types[i]->getName()) != m_knownTokens.end())
      continue;
    m_knownTokens.insert(std::pair<std::string, Token>(types[i]->getName(), Token_Type));
  }

  for(size_t i=0;i<aliases.size();i++)
  {
    if(m_knownTokens.find(aliases[i]->getNewUserName()) != m_knownTokens.end())
      continue;
    m_knownTokens.insert(std::pair<std::string, Token>(aliases[i]->getNewUserName(), Token_Type));
  }

  for(size_t i=0;i<functions.size();i++)
  {
    if(functions[i]->getName().length() <= 2)
      continue;
    if(m_knownTokens.find(functions[i]->getName()) != m_knownTokens.end())
      continue;
    m_knownTokens.insert(std::pair<std::string, Token>(functions[i]->getName(), Token_Function));
  }
}
