/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRIC_SplitSearch_hpp
#define _FABRIC_SplitSearch_hpp

#if defined(_MSC_VER) || defined(SWIGWIN)
# if defined(__cplusplus)
#  define FABRICSERVICES_SPLITSEARCH_IMPORT extern "C" __declspec(dllimport)
#  define FABRICSERVICES_SPLITSEARCH_EXPORT extern "C" __declspec(dllexport)
# else
#  define FABRICSERVICES_SPLITSEARCH_IMPORT __declspec(dllimport)
#  define FABRICSERVICES_SPLITSEARCH_EXPORT __declspec(dllexport)
# endif
#else
# if defined(SWIG)
#  define FABRICSERVICES_SPLITSEARCH_IMPORT extern "C"
#  define FABRICSERVICES_SPLITSEARCH_EXPORT extern "C"
# else
#  define FABRICSERVICES_SPLITSEARCH_IMPORT extern "C" __attribute__ ((visibility("default")))
#  define FABRICSERVICES_SPLITSEARCH_EXPORT extern "C" __attribute__ ((visibility("default")))
# endif
#endif

#if defined(FABRICSERVICES_SPLITSEARCH_BUILDING)
# define FABRICSERVICES_SPLITSEARCH_DECL FABRICSERVICES_SPLITSEARCH_EXPORT
#else
# define FABRICSERVICES_SPLITSEARCH_DECL FABRICSERVICES_SPLITSEARCH_IMPORT
#endif

typedef void *FabricServices_SplitSearch_Matches;

FABRICSERVICES_SPLITSEARCH_DECL
unsigned FabricServices_SplitSearch_Matches_GetSize(
  FabricServices_SplitSearch_Matches _matches
  );

FABRICSERVICES_SPLITSEARCH_DECL
void const *FabricServices_SplitSearch_Matches_GetUserdata(
  FabricServices_SplitSearch_Matches _matches,
  unsigned index
  );

FABRICSERVICES_SPLITSEARCH_DECL
unsigned FabricServices_SplitSearch_Matches_GetUserdatas(
  FabricServices_SplitSearch_Matches _matches,
  unsigned max,
  void const **userdatas
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_KeepFirst(
  FabricServices_SplitSearch_Matches _matches,
  unsigned count
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_Select(
  FabricServices_SplitSearch_Matches _matches,
  unsigned index
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_Retain(
  FabricServices_SplitSearch_Matches _matches
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Matches_Release(
  FabricServices_SplitSearch_Matches _matches
  );

typedef void *FabricServices_SplitSearch_Dict;

FABRICSERVICES_SPLITSEARCH_DECL
FabricServices_SplitSearch_Dict FabricServices_SplitSearch_Dict_Create(
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Retain(
  FabricServices_SplitSearch_Dict dict
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Release(
  FabricServices_SplitSearch_Dict dict
  );

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Add(
  FabricServices_SplitSearch_Dict dict,
  unsigned numCStrs,
  char const * const *cStrs,
  void const *userdata,
  unsigned echelon,
  unsigned priority
  );

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Add_Delimited(
  FabricServices_SplitSearch_Dict dict,
  char const *delimitedCStr,
  char delimiter,
  void const *userdata,
  unsigned echelon,
  unsigned priority
  );

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Remove(
  FabricServices_SplitSearch_Dict dict,
  unsigned numCStrs,
  char const * const *cStrs,
  void const *userdata
  );

FABRICSERVICES_SPLITSEARCH_DECL
bool FabricServices_SplitSearch_Dict_Remove_Delimited(
  FabricServices_SplitSearch_Dict dict,
  char const *delimitedCStr,
  char delimiter,
  void const *userdata
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_Clear(
  FabricServices_SplitSearch_Dict dict
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_LoadPrefs(
  FabricServices_SplitSearch_Dict dict,
  char const *filename
  );

FABRICSERVICES_SPLITSEARCH_DECL
void FabricServices_SplitSearch_Dict_SavePrefs(
  FabricServices_SplitSearch_Dict dict,
  char const *filename
  );

FABRICSERVICES_SPLITSEARCH_DECL
FabricServices_SplitSearch_Matches FabricServices_SplitSearch_Dict_Search(
  FabricServices_SplitSearch_Dict _dict,
  unsigned numCStrs,
  char const * const *cStrs
  );

namespace FabricServices { namespace SplitSearch {

class Dict;

class Matches
{
  friend class Dict;

  FabricServices_SplitSearch_Matches _matches;

  Matches( FabricServices_SplitSearch_Matches _matches_in ) :
    _matches( _matches_in ) {}

public:

  Matches() : _matches( 0 ) {}

  Matches( Matches const &that ) : _matches( that._matches )
  {
    if ( _matches )
      FabricServices_SplitSearch_Matches_Retain( _matches );
  }

#if FTL_HAS_RVALUE_REFERENCES
  Matches( Matches &&that )
    : _matches( that._matches )
  {
    that._matches = 0;
  }
#endif
  
  Matches &operator=( Matches const &that )
  {
    if ( _matches != that._matches )
    {
      if ( _matches )
        FabricServices_SplitSearch_Matches_Release( _matches );
      _matches = that._matches;
      if ( _matches )
        FabricServices_SplitSearch_Matches_Retain( _matches );
    }
    return *this;
  }
  
#if FTL_HAS_RVALUE_REFERENCES
  Matches &operator=( Matches &&that )
  {
    if ( _matches )
      FabricServices_SplitSearch_Matches_Release( _matches );
    _matches = that._matches;
    that._matches = 0;
    return *this;
  }
#endif

  ~Matches()
  {
    if ( _matches )
      FabricServices_SplitSearch_Matches_Release( _matches );
  }

  void clear()
  {
    if ( _matches )
    {
      FabricServices_SplitSearch_Matches_Release( _matches );
      _matches = 0;
    }
  }

  unsigned getSize() const
  {
    if ( _matches )
      return FabricServices_SplitSearch_Matches_GetSize( _matches );
    else
      return 0;
  }

  void const *getUserdata( unsigned index ) const
  {
    if ( _matches )
      return FabricServices_SplitSearch_Matches_GetUserdata( _matches, index );
    else
      return 0;
  }

  unsigned getUserdatas( unsigned max, void const **userdatas ) const
  {
    if ( _matches )
      return FabricServices_SplitSearch_Matches_GetUserdatas(
        _matches, max, userdatas
        );
    else
      return 0;
  }

  void select(
    unsigned matchIndex
    )
  {
    if ( _matches )
      FabricServices_SplitSearch_Matches_Select( _matches, matchIndex );
  }

  void keepFirst( unsigned count )
  {
    if ( _matches )
      FabricServices_SplitSearch_Matches_KeepFirst( _matches, count );
  }
};

class Dict
{
  FabricServices_SplitSearch_Dict _dict;

public:

  Dict()
  {
    _dict = FabricServices_SplitSearch_Dict_Create();
  }

  Dict( Dict const &that ) : _dict( that._dict )
  {
    if ( _dict )
      FabricServices_SplitSearch_Dict_Retain( _dict );
  }

  Dict &operator=( Dict const &that )
  {
    if ( _dict != that._dict )
    {
      FabricServices_SplitSearch_Dict_Release( _dict );
      _dict = that._dict;
      FabricServices_SplitSearch_Dict_Retain( _dict );
    }
    return *this;
  }

  ~Dict()
  {
    if ( _dict )
      FabricServices_SplitSearch_Dict_Release( _dict );
  }

  bool add(
    unsigned numCStrs,
    char const * const *cStrs,
    void const *userdata,
    unsigned echelon = 0,
    unsigned priority = 0
    )
  {
    return FabricServices_SplitSearch_Dict_Add(
      _dict,
      numCStrs,
      cStrs,
      userdata,
      echelon,
      priority
      );
  }

  bool add(
    char const *delimitedCStr,
    char delimiter,
    void const *userdata,
    unsigned echelon = 0,
    unsigned priority = 0
    )
  {
    return FabricServices_SplitSearch_Dict_Add_Delimited(
      _dict,
      delimitedCStr,
      delimiter,
      userdata,
      echelon,
      priority
      );
  }

  bool remove(
    unsigned numCStrs,
    char const * const *cStrs,
    void const *userdata
    )
  {
    return FabricServices_SplitSearch_Dict_Remove(
      _dict,
      numCStrs,
      cStrs,
      userdata
      );
  }

  bool remove(
    char const *delimitedCStr,
    char delimiter,
    void const *userdata
    )
  {
    return FabricServices_SplitSearch_Dict_Remove_Delimited(
      _dict,
      delimitedCStr,
      delimiter,
      userdata
      );
  }

  void clear()
  {
    FabricServices_SplitSearch_Dict_Clear( _dict );
  }

  Matches search(
    unsigned numCStrs,
    char const * const *cStrs
    ) const
  {
    return Matches(
      FabricServices_SplitSearch_Dict_Search( _dict, numCStrs, cStrs )
      );
  }

  void loadPrefs( char const *filename )
  {
    FabricServices_SplitSearch_Dict_LoadPrefs( _dict, filename );
  }

  void savePrefs( char const *filename ) const
  {
    FabricServices_SplitSearch_Dict_SavePrefs( _dict, filename );
  }
};


} }

#endif //_FABRIC_SplitSearch_hpp
