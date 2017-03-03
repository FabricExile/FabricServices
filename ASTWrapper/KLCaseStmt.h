// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLCaseStmt__
#define __ASTWrapper_KLCaseStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLCaseStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLCaseStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

    protected:

      KLCaseStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLCaseStmt__
