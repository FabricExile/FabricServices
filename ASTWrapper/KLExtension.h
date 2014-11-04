// Copyright 2010-2014 Fabric Engine Inc. All rights reserved.

#ifndef __ASTWrapper_KLExtension__
#define __ASTWrapper_KLExtension__

#include "KLDeclContainer.h"
#include "KLFile.h"

namespace FabricServices
{

  namespace ASTWrapper
  {
    // forward decl
    class KLASTManager;

    class KLExtension : public KLDeclContainer
    {
      friend class KLASTManager;
      friend class KLFile;

    public:

      struct Version
      {
        uint32_t major;
        uint32_t minor;
        uint32_t revision;

        bool operator < (const Version & other) const;
        bool operator > (const Version & other) const;
        bool operator == (const Version & other) const;
        bool operator != (const Version & other) const;
      };

      virtual ~KLExtension();

      const KLASTManager * getASTManager() const;
      const char * getName() const;
      const Version & getVersion() const;

      std::vector<const KLFile*> getFiles() const;

      // decl vector getters
      virtual std::vector<const KLRequire*> getRequires() const;
      virtual std::vector<const KLAlias*> getAliases() const;
      virtual std::vector<const KLConstant*> getConstants() const;
      virtual std::vector<const KLType*> getTypes() const;
      virtual std::vector<const KLFunction*> getFunctions() const;
      virtual std::vector<const KLOperator*> getOperators() const;

      // decl vector getter overloads
      virtual std::vector<const KLInterface*> getInterfaces() const;
      virtual std::vector<const KLStruct*> getStructs() const;
      virtual std::vector<const KLObject*> getObjects() const;

    protected:
      
      KLExtension(const KLASTManager* astManager, const char * jsonFilePath);
      KLExtension(const KLASTManager* astManager, const char * name, const char * jsonContent, uint32_t numKLFiles, const char ** klContent);
      void parse();

    private:

      void init(const char * jsonContent, uint32_t numKLFiles, const char ** klContent);
      std::vector<std::string> extractKLFilePaths(JSONData data, const char * extensionName);

      bool m_parsed;
      const KLASTManager* m_astManager;
      std::string m_name;
      Version m_version;
      std::vector<const KLFile*> m_files;
    };

  };

};

#include "KLASTManager.h"

#endif // __ASTWrapper_KLExtension__
