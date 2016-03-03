// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLComment.h"
#include "KLCommented.h"
#include "KLStruct.h"

#include <FTL/MatchChar.h>
#include <FTL/MatchPrefix.h>
#include <FTL/Str.h>
#include <limits.h>

using namespace FabricServices::ASTWrapper;

KLComment::KLComment(const KLFile* klFile, const KLCommented * owner, JSONData data)
: KLDecl(klFile, data)
, m_owner(owner)
{
  gatherDoxygenContent();
}

KLComment::~KLComment()
{
}

KLDeclType KLComment::getDeclType() const
{
  return KLDeclType_Comment;
}

bool KLComment::isOfDeclType(KLDeclType type) const
{
  return type == KLDeclType_Comment;
}

bool KLComment::isInternal() const
{
  return true;
}

void KLComment::gatherDoxygenContent() const
{
  if(m_content.size() > 0)
    return;

  bool inBlock = false;
  for(uint32_t i=0;i<getArraySize();i++)
  {
    const char * content = getStringArrayElement(i);
    if(!content)
      continue;

    std::vector<std::string> lines;
    FTL::StrSplit<'\n'>( content, lines );
    for(uint32_t j=0;j<lines.size();j++)
    {
      std::string l0 = lines[j];
      std::string l1 = lines[j];
      FTL::StrTrimWhitespace( l0 );
      FTL::StrTrim<
        FTL::MatchCharAny<
          FTL::MatchCharSingle<'/'>,
          FTL::MatchCharSingle<'*'>,
          FTL::MatchCharSingle<'\n'>,
          FTL::MatchCharSingle<'\r'>
          >
        >( l1 );

      if(l0.substr(0, 3) == "///")
      {
        if(l1.length() > 0)
          m_content.push_back(l1);
      }
      else if(l0.substr(0, 3) == "/**")
      {
        if(l1.length() > 0)
          m_content.push_back(l1);        
        inBlock = true;
      }
      else if(inBlock)
      {
        m_content.push_back(l1);
      }
    }
  }
}

bool KLComment::isEmpty() const
{
  return m_content.size() == 0;
}

bool KLComment::hasQualifier(
  const char * qualifier,
  bool searchParents
  ) const
{
  std::string q;
  if(qualifier)
  {
    q = qualifier;
    FTL::StrToLower(q);
    FTL::StrTrimWhitespace(q);
  }

  if(q.length() == 0)
    return true;

  std::map<std::string, std::string>::iterator it = m_qualifiers.find(q);
  if(it != m_qualifiers.end())
    return true;

  for(uint32_t i=0;i<m_content.size();i++)
  {
    std::string l = m_content[i];
    FTL::StrTrimWhitespace(l);
    FTL::StrToLower(l);
    if(l.substr(0, q.length()+1) == "\\"+q)
      return true;
  }

  // check if the parents have some
  if ( searchParents
    && m_owner->isOfDeclType(KLDeclType_Struct) )
  {
    const KLStruct * klStruct = (const KLStruct *)m_owner;
    std::vector<const KLType*> parents = klStruct->getParents();
    for(size_t i=0;i<parents.size();i++)
    {
      if ( parents[i]->getComments()->hasQualifier(
        qualifier,
        searchParents
        ) )
        return true;
    }
  }

  return false;
}

std::string KLComment::getQualifier(const char * qualifier, const char * defaultResult) const
{
  std::string q;
  if(qualifier)
  {
    q = qualifier;
    FTL::StrToLower(q);
    FTL::StrTrimWhitespace(q);
  }

  std::map<std::string, std::string>::iterator it = m_qualifiers.find(q);
  if(it != m_qualifiers.end())
    return it->second;

  std::vector<std::string> content;
  std::string insideQualifier;

  for(uint32_t i=0;i<m_content.size();i++)
  {
    std::string l = m_content[i];
    FTL::StrTrimWhitespace(l);
    std::string lPrefix = l.substr(0, q.length() + 1);
    FTL::StrToLower(lPrefix);

    if(lPrefix == "\\"+q && q != "")
    {
      std::string l2 = l.substr(q.length()+1, 10000);
      FTL::StrTrimWhitespace(l2);
      content.push_back(l2);
    }

    if(l.substr(0, 1) == "\\" && FTL::StrCount<' '>(l) == 0)
    {
      if(insideQualifier.length() > 0)
      {
        if(l == "\\end"+insideQualifier)
          insideQualifier = "";
      }
      else
      {
        insideQualifier = l.substr(1, 10000);
      }
    }

    if((q == "" || q == "plaintext") && l.substr(0, 1) != "\\" && insideQualifier.length() == 0)
    {
      content.push_back(l);
    }
  }

  std::string result;

  // check if any parent has something for us
  if(content.size() == 0)
  {
    if(m_owner->isOfDeclType(KLDeclType_Struct))
    {
      const KLStruct * klStruct = (const KLStruct *)m_owner;
      std::vector<const KLType*> parents = klStruct->getParents();
      for(size_t i=0;i<parents.size();i++)
      {
        result = parents[i]->getComments()->getQualifier(qualifier);
        if(result.length() > 0)
        {
          m_qualifiers.insert(std::pair<std::string, std::string>(q, result));
          return result;          
        }
      }
    }    
  }

  if(content.size() == 0 && defaultResult)
    content.push_back(defaultResult);

  for(uint32_t i=0;i<content.size();i++)
  {
    if(i>0)
      result += "\n";
    result += content[i];
  }

  m_qualifiers.insert(std::pair<std::string, std::string>(q, result));

  return result;
}

std::string KLComment::getSingleQualifier(const char * qualifier, const char * defaultResult) const
{
  std::string q = getQualifier(qualifier, defaultResult);
  if(q.length() == 0)
    return q;

  std::vector<std::string> lines;
  FTL::StrSplit<'\n'>(q, lines);

  return lines[0];
}

std::string KLComment::getSingleQualifierWithName(const char * qualifier, const char * name, const char * defaultResult) const
{
  std::string q = getQualifier(qualifier, defaultResult);
  if(q.length() == 0)
    return q;

  std::vector<std::string> lines;
  FTL::StrSplit<'\n'>(q, lines);

  for(size_t i=0;i<lines.size();i++)
  {
    size_t pos = lines[i].find(' ');
    if(pos == std::string::npos)
      continue;
    if(lines[i].substr(0, pos) != name)
      continue;
    return lines[i].substr(pos+1, lines[i].length());
  }
  return "";
}

std::string KLComment::getQualifierBracket(const char * qualifier, const char * defaultResult) const
{
  if(!qualifier)
    return "";
  std::string q1 = qualifier;
  FTL::StrToLower(q1);
  FTL::StrTrimWhitespace(q1);

  if(q1.length() == 0)
    return "";

  std::map<std::string, std::string>::iterator it = m_qualifiers.find(q1);
  if(it != m_qualifiers.end())
    return it->second;

  std::string q2 = "end" + q1;

  std::vector<std::string> content;
  bool inBlock = false;
  uint32_t blockIndentation = UINT_MAX;

  for(uint32_t i=0;i<m_content.size();i++)
  {
    std::string l = m_content[i];
    FTL::StrTrimLeft<
      FTL::MatchCharAny<
        FTL::MatchCharSingle<' '>,
        FTL::MatchCharSingle<'\t'>
        >
      >(l);

    if(inBlock)
    {
      if(l.substr(0, q2.length() + 1) == "\\" + q2)
      {
        inBlock = false;
      }
      else
      {
        if(q1 == "rst")
        {
          content.push_back(m_content[i]);
        }
        else
        {
          if(blockIndentation == UINT_MAX)
            blockIndentation = m_content[i].length() - l.length();
          content.push_back(m_content[i].substr(blockIndentation, 10000));
        }
      }
    }
    else
    {
      if(l.substr(0, q1.length() + 1) == "\\" + q1)
      {
        inBlock = true;
        blockIndentation = UINT_MAX;
      }
    }
  }

  if(content.size() == 0 && defaultResult)
    content.push_back(defaultResult);

  std::string result;
  for(uint32_t i=0;i<content.size();i++)
  {
    if(i>0)
      result += "\n";
    result += content[i];
  }

  m_qualifiers.insert(std::pair<std::string, std::string>(q1, result));

  return result;
}

std::string KLComment::removeRstRoles(const char * text)
{
  // [pzion 201450224] Replace eg. ":role:`some text`" by "some text"

  std::string result = text;

  FTL::MatchPrefixSeq<
    FTL::MatchPrefixChar< FTL::MatchCharSingle<':'> >,
    FTL::MatchPrefixOneOrMore<
      FTL::MatchPrefixChar<
        FTL::MatchCharAny<
          FTL::MatchCharRange<'a', 'z'>,
          FTL::MatchCharRange<'A', 'Z'>,
          FTL::MatchCharRange<'0', '9'>,
          FTL::MatchCharSingle<'\\'>,
          FTL::MatchCharSingle<'-'>,
          FTL::MatchCharSingle<'_'>
          >
        >
      >,
    FTL::MatchPrefixChar< FTL::MatchCharSingle<':'> >,
    FTL::MatchPrefixChar< FTL::MatchCharSingle<'`'> >
    > prefixMatchSeq;

  FTL::StrRef resultStr = result;
  FTL::StrRef::IT itBegin = resultStr.begin();
  FTL::StrRef::IT itEnd = resultStr.end();
  FTL::StrRef::IT it = itBegin;
  for (;;)
  {
    FTL::StrRef::IT itPrefixEnd = it;
    if ( prefixMatchSeq( itPrefixEnd, itEnd ) )
    {
      FTL::StrRef::IT itCloseBackTick =
        resultStr.find( itPrefixEnd, itEnd, '`' );
      if ( itCloseBackTick != itEnd )
      {
        size_t headLength = it - itBegin;
        size_t textStart = itCloseBackTick - itBegin;
        size_t textLength = itCloseBackTick - itPrefixEnd;
        memmove( &result[headLength], &result[textStart], textLength );
        size_t tailStart = itCloseBackTick + 1 - itBegin;
        size_t tailLength = itEnd - itCloseBackTick - 1;
        memmove( &result[headLength+textLength], &result[tailStart], tailLength );
        result.resize( headLength + textLength + tailLength );

        resultStr = result;
        itBegin = resultStr.begin();
        itEnd = resultStr.end();
        it = itBegin + tailLength;

        continue;
      }
    }
    ++it;
  }

  return result;
}

bool KLComment::getColorFromSingleQualifier(const char * qualifier, int & r, int & g, int & b) const
{
  if(qualifier == NULL)
    return false;
  std::string colorStr = getSingleQualifier(qualifier);
  if(colorStr.length() == 0)
    return false;

  std::string filteredColorStr;
  int commaCount = 0;
  for(unsigned int i=0;i<colorStr.length();i++)
  {
    char c = colorStr[i];
    if(c == ',')
      commaCount++;
    if(isalnum(c) || c == '(' || c == ')' || c == ',') // don't use '.'' since it is all integers
      filteredColorStr += tolower(c);
  }

  if(filteredColorStr.length() < 12) // color(0,0,0)
  {
    printf("Invalid '%s' color token found: '%s'\n", qualifier, colorStr.c_str());
    return false;
  }
  if(commaCount != 2)
  {
    printf("Invalid '%s' color token found: '%s'\n", qualifier, colorStr.c_str());
    return false;
  }
  if(filteredColorStr.substr(0, 6) != "color(")
  {
    printf("Invalid '%s' color token found: '%s'\n", qualifier, colorStr.c_str());
    return false;
  }
  if(filteredColorStr[filteredColorStr.length()-1] != ')')
  {
    printf("Invalid '%s' color token found: '%s'\n", qualifier, colorStr.c_str());
    return false;
  }

  std::string parts = filteredColorStr.substr(6, filteredColorStr.length() - 7);

  int firstComma = parts.find(',');
  int secondComma = parts.find(',', firstComma + 1);

  std::string rStr = parts.substr(0, firstComma);
  std::string gStr = parts.substr(firstComma + 1, secondComma - firstComma - 1);
  std::string bStr = parts.substr(secondComma + 1, parts.length() - secondComma);

  r = atoi(rStr.c_str());
  g = atoi(gStr.c_str());
  b = atoi(bStr.c_str());

  return true;
}

std::string KLComment::getPlainText() const
{
  return getQualifier(NULL, "");
}

std::string KLComment::getBrief() const
{
  return getSingleQualifier("brief", "");
}

std::string KLComment::getCategory() const
{
  return getSingleQualifier("category", "");
}

std::string KLComment::getRst() const
{
  return getQualifierBracket("rst", "");
}

std::string KLComment::getExample() const
{
  return getQualifierBracket("example", "");
}

std::string KLComment::getContent() const
{
  std::string result;
  for(size_t i=0;i<m_content.size();i++)
  {
    if(i > 0)
      result += "\n";
    result += m_content[i];
  }
  return result;
}

std::vector<std::string> KLComment::getContentAsVector() const
{
  return m_content;
}

void KLComment::appendToContent(std::vector<std::string> content) const
{
  m_content.insert(m_content.end(), content.begin(), content.end());
}
