// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLType__
#define __ASTWrapper_KLType__

#include "KLCommented.h"
#include "KLMethod.h"
#include "KLTypeOp.h"

#include <string>
#include <map>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLType : public KLCommented
    {
      friend class KLNameSpace;

    public:

      virtual ~KLType();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getName() const;
      std::string getNameWithNS() const;
      virtual const char * getKLType() const = 0;
      virtual std::vector<const KLType*> getParents() const = 0;

      virtual bool isForwardDecl() const = 0;

      uint32_t getMethodCount() const;
      const KLMethod * getMethod(uint32_t index) const;
      const KLMethod * getMethod(const char * labelOrName) const;
      std::vector<const KLMethod*> getMethods(bool includeInherited = false, bool includeInternal = true, const char * category = 0, bool sorted = true) const;

      uint32_t getTypeOpCount() const;
      const KLTypeOp * getTypeOp(uint32_t index) const;
      const KLTypeOp * getTypeOp(const char * labelOrName) const;
      std::vector<const KLTypeOp*> getTypeOps() const;

    protected:

      KLType(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data);
      bool pushMethod(KLMethod * method) const;
      bool pushTypeOp(KLTypeOp * typeOp) const;
      mutable std::vector<KLMethod*> m_methods;
      mutable std::map<std::string, uint32_t> m_methodLabelToId;
      mutable std::vector<const KLTypeOp*> m_typeOps;
      mutable std::map<std::string, uint32_t> m_typeOpLabelToId;

    private:
      
      std::string m_name;
    };

  };

};


#endif // __ASTWrapper_KLType__
