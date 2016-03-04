// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLConditionalStmt__
#define __ASTWrapper_KLConditionalStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLConditionalStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLConditionalStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const KLStmt * getTrueStatement() const;
      const KLStmt * getFalseStatement() const;

    protected:

      KLConditionalStmt(const KLFile* klFile, JSONData data, KLStmt * parent = NULL);
    };

  };

};


#endif // __ASTWrapper_KLConditionalStmt__
