// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLComment__
#define __ASTWrapper_KLComment__

#include "KLDecl.h"

#include <string>
#include <map>
#include <vector>

namespace FabricServices
{

  namespace ASTWrapper
  {
    // forward declarations
    class KLCommented;

    class KLComment : public KLDecl
    {
      friend class KLCommented;
      friend class KLExtension;
      
    public:
      
      virtual ~KLComment();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;

      virtual bool isInternal() const;
      bool isEmpty() const;
      bool hasQualifier(
        const char * qualifier,
        bool searchParents = true
        ) const;
      std::string getQualifier(const char * qualifier = 0, const char * defaultResult = 0) const;
      std::string getSingleQualifier(const char * qualifier, const char * defaultResult = 0) const;
      std::string getSingleQualifierWithName(const char * qualifier, const char * name, const char * defaultResult = 0) const;
      std::string getQualifierBracket(const char * qualifier, const char * defaultResult = 0) const;
      static std::string removeRstRoles(const char * text);
      bool getColorFromSingleQualifier(const char * qualifier, int & r, int & g, int & b) const;

      std::string getPlainText() const;
      std::string getBrief() const;
      std::string getCategory() const;
      std::string getRst() const;
      std::string getExample() const;
      std::string getContent() const;
      std::vector<std::string> getContentAsVector() const;

    protected:

      KLComment(const KLFile* klFile, const KLNameSpace * nameSpace, const KLCommented * owner, JSONData data);
      void appendToContent(std::vector<std::string> content) const;

    private:

      void gatherDoxygenContent() const;

      const KLCommented * m_owner;
      mutable std::vector<std::string> m_content;
      mutable std::map<std::string, std::string> m_qualifiers;
    };

  };

};

#include "KLCommented.h"

#endif // __ASTWrapper_KLComment__
