// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLObject__
#define __ASTWrapper_KLObject__

#include "KLStruct.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLObject : public KLStruct
    {
      friend class KLFile;

    public:

      virtual ~KLObject();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual const char * getKLType() const;
      virtual std::vector<const KLType*> getParents() const;

    protected:

      KLObject(const KLFile* klFile, JSONData data);

    private:
      std::vector<std::string> m_parentsAndInterfaces;
    };

  };

};


#endif // __ASTWrapper_KLObject__
