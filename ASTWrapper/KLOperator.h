// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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
      friend class KLFile;

    public:

      virtual ~KLOperator();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual const char * getKLType() const;

    protected:

      KLOperator(const KLFile* klFile, JSONData data);
    };

  };

};


#endif // __ASTWrapper_KLOperator__
