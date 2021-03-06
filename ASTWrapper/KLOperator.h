// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLOperator__
#define __ASTWrapper_KLOperator__

#include "KLFunction.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLOperator : public KLFunction
    {
      friend class KLNameSpace;

    public:

      virtual ~KLOperator();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual const char * getKLType() const;

    protected:

      KLOperator(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data);
    };

  };

};


#endif // __ASTWrapper_KLOperator__
