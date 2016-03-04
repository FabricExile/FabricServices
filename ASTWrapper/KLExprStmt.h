// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLExprStmt__
#define __ASTWrapper_KLExprStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLExprStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLExprStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

    protected:

      KLExprStmt(const KLFile* klFile, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLExprStmt__
