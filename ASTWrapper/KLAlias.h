// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLAlias__
#define __ASTWrapper_KLAlias__

#include "KLCommented.h"

#include <string>
#include <map>
#include <vector>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLAlias : public KLCommented
    {
      friend class KLNameSpace;

    public:

      virtual ~KLAlias();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getNewUserName() const;
      const std::string & getOldUserName() const;

    protected:
      
      KLAlias(const KLFile* klFile, const KLNameSpace * nameSpace, JSONData data);

    private:
      
      std::string m_newUserName;
      std::string m_oldUserName;
    };

  };

};


#endif // __ASTWrapper_KLAlias__
