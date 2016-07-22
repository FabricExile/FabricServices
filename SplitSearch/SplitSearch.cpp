/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#include "SplitSearch.hpp"
#include <FTL/FS.h>
#include <FTL/JSONValue.h>

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringMap.h>
#include <stdint.h>
#include <stdio.h>
#include <streambuf>
#include <string>
#include <vector>

namespace FabricServices { namespace SplitSearch { namespace Impl {

template<typename ArrayTy>
void SplitDelimitedString(
  llvm::StringRef delimitedStr,
  char delimiter,
  ArrayTy &result
  )
{
  std::pair<llvm::StringRef, llvm::StringRef> split(
    llvm::StringRef(), delimitedStr
    );
  while ( !split.second.empty() )
  {
    split = split.second.split( delimiter );
    result.push_back( split.first );
  }
}

static inline unsigned CommonSuffixLength(
  llvm::StringRef lhs,
  llvm::StringRef rhs
  )
{
  unsigned length = 0;
  for (;;)
  {
    if ( length >= lhs.size()
      || length >= rhs.size()
      || tolower( lhs[lhs.size()-length-1] )
        != tolower( rhs[rhs.size()-length-1] ) )
      break;
    ++length;
  }
  return length;
}

struct Score
{
  uint64_t points;
  uint64_t penalty;

  Score()
    : points( 0 )
    , penalty( 0 )
    {}

  Score( uint64_t _points, uint64_t _penalty )
    : points( _points ), penalty( _penalty ) {}

  bool isValid() const
    { return points != UINT64_MAX && penalty != UINT64_MAX; }

  static Score Invalid()
    { return Score( UINT64_MAX, UINT64_MAX ); }

  Score &operator+=( Score const &that )
  {
    points += that.points;
    penalty += that.penalty;
    return *this;
  }

  bool operator<( Score const &that ) const
  {
    return points < that.points
      || ( points == that.points
        && penalty > that.penalty );
  }

  bool operator>( Score const &that ) const
  {
    return points > that.points
      || ( points == that.points
        && penalty < that.penalty );
  }
};

struct RevMatchResult
{
  uint64_t size;
  Score score;

  RevMatchResult()
    : size( 0 )
    {}

  RevMatchResult &operator+=( RevMatchResult const &that )
  {
    size += that.size;
    score += that.score;
    return *this;
  }
};

inline uint64_t Sq( uint64_t x ) { return x * x; }

static inline RevMatchResult RevMatch(
  llvm::StringRef haystack,
  llvm::StringRef needle
  )
{
  RevMatchResult bestResult;
  bestResult.score.penalty = Sq( haystack.size() + 1 );
  uint64_t tail = 0;
  while ( !haystack.empty() )
  {
    RevMatchResult thisResult;
    thisResult.size = CommonSuffixLength( haystack, needle );
    if ( thisResult.size > 0 )
    {
      uint64_t head = haystack.size() - thisResult.size;
      thisResult.score.points = Sq(thisResult.size);
      thisResult.score.penalty = Sq(head + 1) + tail;
      if ( thisResult.size < haystack.size()
        && thisResult.size < needle.size() )
      {
        llvm::StringRef subHaystack(
          haystack.data(), haystack.size() - thisResult.size
          );
        llvm::StringRef subNeedle(
          needle.data(), needle.size() - thisResult.size
          );
        thisResult += RevMatch( subHaystack, subNeedle );
      }
      if ( bestResult.score < thisResult.score )
        bestResult = thisResult;
    }
    haystack = haystack.drop_back();
    ++tail;
  }
  return bestResult;
}

static inline Score ScoreMatch(
  llvm::ArrayRef<llvm::StringRef> prefixes,
  llvm::ArrayRef<llvm::StringRef> needle
  )
{
  if ( needle.empty() )
    return Score::Invalid();

  llvm::StringRef lastNeedle = needle.back();
  llvm::StringRef lastPrefix = prefixes.back();
  needle = needle.drop_back();
  RevMatchResult revMatch = RevMatch( lastPrefix, lastNeedle );

  Score subScore;
  llvm::StringRef subLastNeedle =
    lastNeedle.drop_back( revMatch.size );
  if ( !needle.empty() || !subLastNeedle.empty() )
  {
    if ( prefixes.size() > 1 )
    {
      llvm::SmallVector<llvm::StringRef, 8> subNeedle;
      subNeedle.append( needle.begin(), needle.end() );
      if ( !subLastNeedle.empty() )
        subNeedle.push_back( subLastNeedle );
      
      llvm::ArrayRef<llvm::StringRef> subPrefixes = prefixes.drop_back();
      subScore = ScoreMatch( subPrefixes, subNeedle );
    }
    else subScore = Score::Invalid();
  }

  if ( subScore.isValid() )
    return Score(
      revMatch.score.points + subScore.points/2,
      revMatch.score.penalty + subScore.penalty/2
      );
  else
    return Score::Invalid();
}

class Match
{
  void const *m_userdata;
  Score m_score;
  int m_priority;

public:

  Match( void const *userdata, Score score, int priority ) :
    m_userdata( userdata ), m_score( score ), m_priority( priority ) {}

  void const *getUserdata() const { return m_userdata; }

  void dump( size_t index )
  {
    printf( "index=%u score.points=%u score.penalty=%u priority=%d userdata=%s\n",
      unsigned( index ),
      unsigned( m_score.points ),
      unsigned( m_score.penalty ),
      m_priority,
      (char const *)m_userdata
      );
  }

  struct LessThan
  {
    bool operator()( Match const &lhs, Match const &rhs )
    {
      return lhs.m_priority > rhs.m_priority
        || ( lhs.m_priority == rhs.m_priority
          && lhs.m_score > rhs.m_score );
    }
  };
};

class Shareable
{
  unsigned _refCount;

protected:

  Shareable() : _refCount( 1 ) {}
  virtual ~Shareable() {}

public:

  void retain()
  {
    ++_refCount;
  }

  void release()
  {
    if ( --_refCount == 0 )
      delete this;
  }
};

class Matches : public Shareable
{
  std::vector<Match> m_impl;

  Matches( Matches const & ) = delete;
  Matches &operator=( Matches const & ) = delete;

protected:

  virtual ~Matches() {}

public:

  Matches() {}

  void add( void const *userdata, Score score, int priority )
  {
    m_impl.push_back( Match( userdata, score, priority ) );
  }

  void sort() { std::sort( m_impl.begin(), m_impl.end(), Match::LessThan() ); }

  void dump()
  {
    for ( size_t i = 0; i < m_impl.size(); ++i )
    {
      if ( i >= 20 )
        break;
      m_impl[i].dump( i );
    }
  }

  unsigned getSize() const { return m_impl.size(); }

  unsigned getUserdatas(
    unsigned max,
    void const **userdatas
    ) const
  {
    unsigned index = 0;
    while ( index < max && index < m_impl.size() )
    {
      userdatas[index] = m_impl[index].getUserdata();
      ++index;
    }
    return index;
  }
};

static inline llvm::ArrayRef<llvm::StringRef> DropFront(
  llvm::ArrayRef<llvm::StringRef> strs
  )
{
  return llvm::ArrayRef<llvm::StringRef>( strs.begin() + 1, strs.end() );
}

class Node
{
  void const *m_userdata;
  int m_priority;
  llvm::StringMap< std::unique_ptr<Node> > m_children;

protected:

  void search(
    llvm::SmallVector<llvm::StringRef, 8> &prefixes,
    llvm::ArrayRef<llvm::StringRef> needle,
    Matches *matches
    ) const
  {
    for ( llvm::StringMap< std::unique_ptr<Node> >::const_iterator it =
      m_children.begin(); it != m_children.end(); ++it )
    {
      prefixes.push_back( it->first() );

      if ( it->second->m_userdata )
      {
        Score score = ScoreMatch( prefixes, needle );
        if ( score.isValid() )
          matches->add( it->second->m_userdata, score, it->second->m_priority );
      }

      std::unique_ptr<Node> const &node = it->second;
      node->search( prefixes, needle, matches );

      prefixes.pop_back();
    }
  }

public:

  Node( void *userdata ) : m_userdata( userdata ), m_priority( -1 ) {}
  Node( Node const & ) = delete;
  Node &operator=( Node const & ) = delete;
  ~Node() {}

  bool add(
    llvm::ArrayRef<llvm::StringRef> strs,
    void const *userdata
    )
  {
    if ( !strs.empty() )
    {
      std::unique_ptr<Node> &child = m_children[strs.front()];
      if ( !child )
        child = std::unique_ptr<Node>( new Node( nullptr ) );
      return child->add( DropFront( strs ), userdata );
    }
    else
    {
      if ( !m_userdata )
        m_userdata = userdata;
      return m_userdata == userdata;
    }
  }

  bool remove(
    llvm::ArrayRef<llvm::StringRef> strs,
    void const *userdata
    )
  {
    if ( !strs.empty() )
    {
      std::unique_ptr<Node> &child = m_children[strs.front()];
      if ( !child )
        return false;
      return child->remove( DropFront( strs ), userdata );
    }
    else
    {
      bool result = m_userdata == userdata;
      m_userdata = nullptr;
      return result;
    }
  }

  void clear()
  {
    m_children.clear();
  }

  void search(
    llvm::ArrayRef<llvm::StringRef> needle,
    Matches *matches
    ) const
  {
    llvm::SmallVector<llvm::StringRef, 8> prefixes;
    search( prefixes, needle, matches );
  }

  int loadPrefsFromJSON( FTL::JSONObject const *jsonObject )
  {
    int highest = m_priority = jsonObject->getSInt32OrDefault( FTL_STR("priority"), -1 );
    if ( FTL::JSONObject const *childJSONObject = jsonObject->maybeGetObject( FTL_STR("children") ) )
    {
      for ( FTL::JSONObject::const_iterator it = childJSONObject->begin();
        it != childJSONObject->end(); ++it )
      {
        FTL::StrRef childName = it->first;
        if ( FTL::JSONObject const *childJSONObject = it->second->maybeCastOrNull<FTL::JSONObject>() )
        {
          llvm::StringMap< std::unique_ptr<Node> >::iterator jt =
            m_children.find( llvm::StringRef( childName.data(), childName.size() ) );
          if ( jt != m_children.end() )
            highest = std::max(
              highest,
              jt->second->loadPrefsFromJSON( childJSONObject )
              );
        }
      }
    }
    return highest;
  }

  FTL::JSONObject *savePrefsToJSON()
  {
    FTL::OwnedPtr<FTL::JSONObject> childrenJSONObject( new FTL::JSONObject );
    for ( llvm::StringMap< std::unique_ptr<Node> >::const_iterator it =
      m_children.begin(); it != m_children.end(); ++it )
    {
      FTL::OwnedPtr<FTL::JSONObject> childPrefs( it->second->savePrefsToJSON() );
      if ( !childPrefs->empty() )
        childrenJSONObject->insert(
          FTL::StrRef( it->first().data(), it->first().size() ),
          childPrefs.take()
          );
    }

    FTL::JSONObject *resultJSONObject = new FTL::JSONObject;
    if ( m_priority != -1 )
      resultJSONObject->insert( FTL_STR("priority"), new FTL::JSONSInt32( m_priority ) );
    if ( !childrenJSONObject->empty() )
      resultJSONObject->insert( FTL_STR("children"), childrenJSONObject.take() );
    return resultJSONObject;
  }
};

class Dict : public Shareable
{
  Node m_root;
  int m_nextPriority;

  Dict( Dict const & ) = delete;
  Dict &operator=( Dict const & ) = delete;

protected:

  virtual ~Dict() {}

public:

  Dict() : m_root( nullptr ), m_nextPriority( 0 ) {}

  bool add(
    llvm::ArrayRef<llvm::StringRef> strs,
    void const *userdata
    )
  {
    return m_root.add( strs, userdata );
  }

  bool remove(
    llvm::ArrayRef<llvm::StringRef> strs,
    void const *userdata
    )
  {
    return m_root.remove( strs, userdata );
  }

  void clear()
  {
    m_root.clear();
  }

  Matches *search( llvm::ArrayRef<llvm::StringRef> needle ) const
  {
    if ( needle.empty() )
      return nullptr;

    Matches *matches = new Matches;
    m_root.search( needle, matches );
    matches->sort();
    // matches->dump();
    return matches;
  }

  void loadPrefs( char const *filename )
  {
    if ( FTL::FSExists( filename ) )
    {
      try
      {
        std::ifstream file( filename );
        std::string jsonStr = std::string(
          std::istreambuf_iterator<char>( file ),
          std::istreambuf_iterator<char>()
          );
        FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
        for (;;)
        {
          try
          {
            FTL::OwnedPtr<FTL::JSONValue> jsonValue(
              FTL::JSONValue::Decode( jsonStrWithLoc )
              );
            if ( !jsonValue )
              break;
            m_nextPriority = m_root.loadPrefsFromJSON(
              jsonValue->cast<FTL::JSONObject>()
              ) + 1;
          }
          catch ( FTL::JSONException e )
          {
            std::cerr
              << "'" << filename << "': Caught exception: "
              << e.getDesc()
              << "\n";
          }
        }
      }
      catch ( ... )
      {
        std::cerr << "'" << filename << "': Unable to load";
      }
    }
  }

  void savePrefs( char const *filename )
  {
    try
    {
      FTL::OwnedPtr<FTL::JSONObject> prefs( new FTL::JSONObject );
      prefs->insert( FTL_STR("nodes"), m_root.savePrefsToJSON() );

      std::ofstream outFile( filename );
      outFile << prefs->encode() << '\n';
    }
    catch ( ... )
    {
      std::cerr << "'" << filename << "': Unable to save";
    }
  }
};

} } }

using namespace FabricServices::SplitSearch::Impl;

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_Retain(
  FabricServices_SplitSearch_Matches _matches
  )
{
  Matches *matches = static_cast<Matches *>( _matches );
  matches->retain();
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_Release(
  FabricServices_SplitSearch_Matches _matches
  )
{
  Matches *matches = static_cast<Matches *>( _matches );
  matches->release();
}

FABRICSERVICES_SPLITSEARCH_DECL
unsigned FabricServices_SplitSearch_Matches_GetSize(
  FabricServices_SplitSearch_Matches _matches
  )
{
  Matches *matches = static_cast<Matches *>( _matches );
  return matches->getSize();
}

FABRICSERVICES_SPLITSEARCH_DECL
unsigned FabricServices_SplitSearch_Matches_GetUserdatas(
  FabricServices_SplitSearch_Matches _matches,
  unsigned max,
  void const **userdatas
  )
{
  Matches *matches = static_cast<Matches *>( _matches );
  return matches->getUserdatas( max, userdatas );
}

FABRICSERVICES_SPLITSEARCH_DECL
FabricServices_SplitSearch_Dict FabricServices_SplitSearch_Dict_Create(
  )
{
  return new Dict;
}

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Add(
  FabricServices_SplitSearch_Dict _dict,
  unsigned numCStrs,
  char const * const *cStrs,
  void const *userdata
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  llvm::SmallVector<llvm::StringRef, 8> strs;
  while ( numCStrs-- > 0 )
    strs.push_back( *cStrs++ );
  return dict->add( strs, userdata );
}

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Add_Delimited(
  FabricServices_SplitSearch_Dict _dict,
  char const *delimitedCStr,
  char delimiter,
  void const *userdata
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  llvm::SmallVector<llvm::StringRef, 8> strs;
  SplitDelimitedString( delimitedCStr, delimiter, strs );
  return dict->add( strs, userdata );
}

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Remove(
  FabricServices_SplitSearch_Dict _dict,
  unsigned numCStrs,
  char const * const *cStrs,
  void const *userdata
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  llvm::SmallVector<llvm::StringRef, 8> strs;
  while ( numCStrs-- > 0 )
    strs.push_back( *cStrs++ );
  return dict->remove( strs, userdata );
}

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Remove_Delimited(
  FabricServices_SplitSearch_Dict _dict,
  char const *delimitedCStr,
  char delimiter,
  void const *userdata
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  llvm::SmallVector<llvm::StringRef, 8> strs;
  SplitDelimitedString( delimitedCStr, delimiter, strs );
  return dict->remove( strs , userdata );
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Clear(
  FabricServices_SplitSearch_Dict _dict
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  dict->clear();
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_LoadPrefs(
  FabricServices_SplitSearch_Dict _dict,
  char const *filename
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  dict->loadPrefs( filename );
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_SavePrefs(
  FabricServices_SplitSearch_Dict _dict,
  char const *filename
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  dict->savePrefs( filename );
}

FABRICSERVICES_SPLITSEARCH_DECL
FabricServices_SplitSearch_Matches FabricServices_SplitSearch_Dict_Search(
  FabricServices_SplitSearch_Dict _dict,
  unsigned numCStrs,
  char const * const *cStrs
  )
{
  Dict *dict = static_cast<Dict *>( _dict );

  // llvm::StringRef testHaystack[] = {
  //   "Mat44",
  //   "MultiplyVector3"
  // };
  // llvm::StringRef testNeedle[] = {
  //   "mat4mul"
  // };
  // Score testScore = ScoreMatch( testHaystack, testNeedle );
  // (void)testScore;

  llvm::SmallVector<llvm::StringRef, 8> needle;
  for ( unsigned i = 0; i < numCStrs; ++i )
    needle.push_back( cStrs[i] );
  return dict->search( needle );
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Retain(
  FabricServices_SplitSearch_Dict _dict
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  dict->retain();
}

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Release(
  FabricServices_SplitSearch_Dict _dict
  )
{
  Dict *dict = static_cast<Dict *>( _dict );
  dict->release();
}
