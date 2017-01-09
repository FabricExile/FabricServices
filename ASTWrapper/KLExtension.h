// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

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
      KLASTManager * getASTManager();
      const char * getName() const;
      const char * getFilePath() const;
      const Version & getVersion() const;

      FabricCore::DFGExec *getDFGExec() const
      {
        return m_dfgExec;
      }

      std::vector<const KLFile*> getFiles() const;

      // decl vector getters
      virtual std::vector<const KLRequire*> getRequires() const;
      virtual std::vector<const KLAlias*> getAliases() const;
      virtual std::vector<const KLConstant*> getConstants() const;
      virtual std::vector<const KLType*> getTypes() const;
      virtual std::vector<const KLFunction*> getFunctions() const;
      virtual std::vector<const KLMethod*> getMethods() const;
      virtual std::vector<const KLOperator*> getOperators() const;

      // decl vector getter overloads
      virtual std::vector<const KLInterface*> getInterfaces() const;
      virtual std::vector<const KLStruct*> getStructs() const;
      virtual std::vector<const KLObject*> getObjects() const;

    protected:
      
      KLExtension(const KLASTManager* astManager, const char * jsonFilePath, FabricCore::DFGExec *dfgExec);
      KLExtension(const KLASTManager* astManager, const char * name, const char * jsonContent, uint32_t numKLFiles, const char ** klContent, FabricCore::DFGExec *dfgExec);

      void parse();
      void storeForwardDeclComments(const KLType * klType);
      void consumeForwardDeclComments(const KLType * klType);

    private:

      void init(const char * jsonContent, uint32_t numKLFiles, const char ** klContent);
      std::vector<std::string> extractKLFilePaths(JSONData data, const char * extensionName);

      bool m_parsed;
      KLASTManager* m_astManager;
      std::string m_name;
      std::string m_filePath;
      Version m_version;
      std::vector<const KLFile*> m_files;
      std::map< std::string, std::vector< std::string > > m_forwardDeclComments;
      FabricCore::DFGExec *m_dfgExec;
    };

  };

};

#include "KLASTManager.h"

#endif // __ASTWrapper_KLExtension__
