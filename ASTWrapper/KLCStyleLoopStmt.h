// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLCStyleLoopStmt__
#define __ASTWrapper_KLCStyleLoopStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLCStyleLoopStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLCStyleLoopStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

    protected:

      KLCStyleLoopStmt(const KLFile* klFile, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLCStyleLoopStmt__
