// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLLocation__
#define __ASTWrapper_KLLocation__

#include "KLDecl.h"

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLLocation
    {
      friend class KLDecl;

    public:

      virtual ~KLLocation();

      uint32_t getLine() const;
      uint32_t getColumn() const;
      uint32_t getEndLine() const;
      uint32_t getEndColumn() const;

    protected:
      
      KLLocation(JSONData data);

    private:
      
      uint32_t m_line;
      uint32_t m_column;
      uint32_t m_endLine;
      uint32_t m_endColumn;
    };

  };

};


#endif // __ASTWrapper_KLLocation__
