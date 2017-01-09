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
      friend class KLFile;

    public:

      virtual ~KLConstant();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getName() const;
      const std::string & getType() const;

    protected:
      
      KLConstant(const KLFile* klFile, JSONData data);

    private:
      
      std::string m_name;
      std::string m_type;
    };

  };

};


#endif // __ASTWrapper_KLConstant__
