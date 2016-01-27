// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __CodeCompletion_KLVariable__
#define __CodeCompletion_KLVariable__

#include "KLTypeDesc.h"

namespace FabricServices
{

  namespace CodeCompletion
  {
    class KLVariable : public KLTypeDesc
    {
      friend class KLCodeAssistant;
      
    public:

      virtual ~KLVariable();

      const std::string & getName() const;

    protected:

      KLVariable(const std::string & name, const std::string & type);
      KLVariable(const std::string & name, const std::string & type, const std::string & arrayModifier);

    private:

      std::string m_name;
    };

  };

};


#endif // __CodeCompletion_KLVariable__
