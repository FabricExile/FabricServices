// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLVarDeclStmt__
#define __ASTWrapper_KLVarDeclStmt__

#include "KLStmt.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLVarDeclStmt : public KLStmt
    {
      friend class KLFunction;
      friend class KLStmt;

    public:

      virtual ~KLVarDeclStmt() {}

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      std::string getBaseType() const;
      uint32_t getCount() const;
      const std::string & getName(uint32_t index) const;
      const std::string & getArrayModifier(uint32_t index) const;

    protected:

      KLVarDeclStmt(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data, KLStmt * parent = NULL);

      std::string m_baseType;
      std::vector<std::string> m_names;
      std::vector<std::string> m_arrayModifiers;
    };

  };

};


#endif // __ASTWrapper_KLVarDeclStmt__
