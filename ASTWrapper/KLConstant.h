// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLConstant__
#define __ASTWrapper_KLConstant__

#include "KLCommented.h"

#include <string>
#include <map>
#include <vector>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLConstant : public KLCommented
    {
      friend class KLNameSpace;

    public:

      virtual ~KLConstant();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getName() const;
      std::string getType(bool includeNameSpace = false) const;

    protected:
      
      KLConstant(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data);

    private:
      
      std::string m_name;
      std::string m_type;
    };

  };

};


#endif // __ASTWrapper_KLConstant__
