// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLNameSpace__
#define __ASTWrapper_KLNameSpace__

#include "KLDeclContainer.h"
#include "KLCommented.h"
#include "KLStmtSearch.h"
#include "KLError.h"
#include <vector>

namespace FabricServices
{

  namespace ASTWrapper
  {
    // forward decl
    class KLExtension;

    class KLNameSpace : public KLDeclContainer, public KLStmtSearch, public KLCommented
    {
      friend class KLFile;
      
    public:

      virtual ~KLNameSpace();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const char * getName() const;

      // decl vector getters
      virtual std::vector<const KLRequire*> getRequires() const;
      virtual std::vector<const KLNameSpace*> getNameSpaces() const;
      virtual std::vector<const KLAlias*> getAliases() const;
      virtual std::vector<const KLConstant*> getConstants() const;
      virtual std::vector<const KLType*> getTypes() const;
      virtual std::vector<const KLFunction*> getFunctions() const;
      virtual std::vector<const KLMethod*> getMethods() const;
      virtual std::vector<const KLOperator*> getOperators() const;

      // decl vector getter overloads
      virtual std::vector<const KLInterface*> getInterfaces() const;
      virtual std::vector<const KLStruct*> getStructs() const;
      virtual std::vector<const KLObject*> getObjects() const;

      virtual const KLStmt * getStatementAtCursor(uint32_t line, uint32_t column) const;

    protected:
      
      KLNameSpace(const KLFile * klFile, const KLNameSpace * nameSpace, JSONData data);
      void clear();

      void parseJSON( FabricCore::Variant const *astVariant );

      std::vector<const KLRequire*> m_requires;
      std::vector<const KLNameSpace*> m_nameSpaces;
      std::vector<const KLAlias*> m_aliases;
      std::vector<const KLConstant*> m_constants;
      std::vector<const KLType*> m_types;
      std::vector<const KLFunction*> m_functions;
      std::vector<const KLMethod*> m_methods;
      std::vector<const KLOperator*> m_operators;

    private:

      std::string m_name;
    };

  };

};

#include "KLExtension.h"

#endif // __ASTWrapper_KLNameSpace__
