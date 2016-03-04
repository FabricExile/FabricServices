// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLParameter__
#define __ASTWrapper_KLParameter__

#include "KLDecl.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLParameter : public KLDecl
    {
      friend class KLFunction;

    public:

      virtual ~KLParameter();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual bool isInternal() const { return false; }

      const std::string & getUsage() const;
      const std::string & getName() const;
      const std::string & getType() const;
      std::string getTypeNoArray() const;
      std::string getTypeArraySuffix() const;

    protected:

      KLParameter(const KLFile* klFile, JSONData data);

    private:
      
      std::string m_usage;
      std::string m_name;
      std::string m_type;
    };

  };

};


#endif // __ASTWrapper_KLParameter__
