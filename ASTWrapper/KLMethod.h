// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLMethod__
#define __ASTWrapper_KLMethod__

#include "KLFunction.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLMethod : public KLFunction
    {
      friend class KLFile;
      friend class KLInterface;
      
    public:

      virtual ~KLMethod();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      const std::string & getThisType() const;
      const std::string & getThisUsage() const;

      virtual bool hasUniqueName() const;
      virtual bool isMethod() const;
      virtual bool isVirtual() const;
      virtual bool isConstructor() const;
      virtual std::string getPrefix() const;
      virtual std::string getSuffix() const;
      virtual const KLComment * getComments() const;

    protected:

      KLMethod(const KLFile* klFile, JSONData data, const std::string & thisType = "");

    private:
      
      std::string m_thisType;
      std::string m_thisUsage;
      mutable int m_isVirtual;
    };

  };

};


#endif // __ASTWrapper_KLMethod__
