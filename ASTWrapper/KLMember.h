// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLMember__
#define __ASTWrapper_KLMember__

#include "KLCommented.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLMember : public KLCommented
    {
      friend class KLStruct;
      
    public:

      virtual ~KLMember();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getName() const;
      const std::string & getType() const;

    protected:

      KLMember(const KLFile* klFile, JSONData data);

    private:
      
      std::string m_name;
      std::string m_type;
    };

  };

};


#endif // __ASTWrapper_KLMember__
