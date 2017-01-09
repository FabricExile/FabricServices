// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLStruct__
#define __ASTWrapper_KLStruct__

#include "KLType.h"
#include "KLMember.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLStruct : public KLType
    {
      friend class KLFile;

    public:

      virtual ~KLStruct();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual bool isForwardDecl() const { return m_isForwardDecl; }

      virtual const char * getKLType() const;
      virtual std::vector<const KLType*> getParents() const;

      virtual uint32_t getMemberCount(bool includeInherited = true) const;
      virtual const KLMember * getMember(uint32_t index, bool includeInherited = true) const;
      virtual const KLMember * getMember(const char *, bool includeInherited = true) const;

    protected:

      KLStruct(const KLFile* klFile, JSONData data);

    private:
      bool m_isForwardDecl;
      std::string m_parentStructName;
      std::vector<const KLMember*> m_members;
    };

  };

};


#endif // __ASTWrapper_KLStruct__
