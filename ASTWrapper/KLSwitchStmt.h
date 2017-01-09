// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLSwitchStmt__
#define __ASTWrapper_KLSwitchStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLSwitchStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLSwitchStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

    protected:

      KLSwitchStmt(const KLFile* klFile, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLSwitchStmt__
