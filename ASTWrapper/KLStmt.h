// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLStmt_h
#define __ASTWrapper_KLStmt_h

#include "KLCommented.h"
#include "KLStmtSearch.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLStmt : public KLCommented, public KLStmtSearch
    {
    public:

      virtual ~KLStmt();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      std::string getTypeName() const;

      virtual uint32_t getChildCount() const;
      virtual const KLStmt * getChild(uint32_t index) const;
      virtual const KLStmt * getParent() const;
      virtual const KLStmt * getTop() const;
      virtual uint32_t getDepth() const;

      virtual std::vector<const KLStmt*> getAllChildrenOfType(KLDeclType type, bool downwards = false, bool upwards = false) const;

      virtual const KLStmt * getStatementAtCursor(uint32_t line, uint32_t column) const;
      virtual uint32_t getCursorDistance(uint32_t line, uint32_t column) const;

    protected:

      KLStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent = NULL);
      const KLStmt * constructChild(JSONData data);

      std::string m_type;
      KLStmt * m_parent;
      uint32_t m_depth;
      std::vector<KLStmt*> m_statements;
    };

  };

};


#endif // __ASTWrapper_KLStmt_h
