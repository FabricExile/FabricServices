// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLRequire__
#define __ASTWrapper_KLRequire__

#include "KLCommented.h"

#include <string>
#include <map>
#include <vector>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLRequire : public KLCommented
    {
      friend class KLNameSpace;

    public:

      virtual ~KLRequire();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getRequiredExtension() const;
      const std::string & getVersionRange() const;

    protected:
      
      KLRequire(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data);

    private:
      
      std::string m_requiredExtension;
      std::string m_versionRange;
    };

  };

};


#endif // __ASTWrapper_KLRequire__
