// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLError__
#define __ASTWrapper_KLError__

#include <string>
#include "KLDecl.h"

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLError
    {
      friend class KLFile;

    public:

      virtual ~KLError();

      const char * getFileName() const;
      int getLine() const;
      int getColumn() const;
      const char * getLevel() const;
      const char * getDesc() const;

    protected:
      
      KLError(JSONData data);

    private:
      
      std::string m_fileName;
      int m_line;
      int m_column;
      std::string m_level;
      std::string m_desc;
    };

  };

};


#endif // __ASTWrapper_KLError__
