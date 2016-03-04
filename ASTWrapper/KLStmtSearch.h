// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLStmtSearch__
#define __ASTWrapper_KLStmtSearch__

namespace FabricServices
{

  namespace ASTWrapper
  {
    // forward decl
    class KLStmt;

    class KLStmtSearch
    {
      virtual const KLStmt * getStatementAtCursor(uint32_t line, uint32_t column) const = 0;

    public:
      virtual ~KLStmtSearch() {};
    };
    
  };

};

#endif // __ASTWrapper_KLStmtSearch__
