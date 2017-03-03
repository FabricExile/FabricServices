// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLCompoundStmt__
#define __ASTWrapper_KLCompoundStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLCompoundStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLCompoundStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

    protected:

      KLCompoundStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLCompoundStmt__
