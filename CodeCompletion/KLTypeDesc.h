// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __CodeCompletion_KLTypeDesc__
#define __CodeCompletion_KLTypeDesc__

#include <string>

namespace FabricServices
{

  namespace CodeCompletion
  {
    class KLTypeDesc
    {
      friend class KLCodeAssistant;
      
    public:

      KLTypeDesc(const std::string & type);
      KLTypeDesc(const std::string & type, const std::string & arrayModifier);
      virtual ~KLTypeDesc();

      const std::string & getType() const;
      std::string getBaseType() const;
      const std::string & getArrayModifier() const;
      bool isArray() const;
      bool isVariableArray() const;
      bool isExternalArray() const;
      bool isDict() const;
      bool isRef() const;

    private:

      std::string m_type;
      std::string m_arrayModifier;
    };

  };

};


#endif // __CodeCompletion_KLTypeDesc__
