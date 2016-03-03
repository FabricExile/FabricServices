// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLASTManager__
#define __ASTWrapper_KLASTManager__

#include "KLDeclContainer.h"
#include "KLLocation.h"
#include "KLExtension.h"

namespace FabricServices
{

  namespace ASTWrapper
  {
    class KLASTClient;

    class KLASTManager : public KLDeclContainer
    {
      friend class KLDecl;
      friend class KLFile;
      friend class KLExtension;
      friend class KLASTClient;

    public:

      KLASTManager(const FabricCore::Client * client);
      virtual ~KLASTManager();

      static KLASTManager * retainGlobalManager(const FabricCore::Client * client);
      static void releaseGlobalManager();
      const FabricCore::Client* getClient() const;

      bool getAutoLoadExtensions() const;
      void setAutoLoadExtensions(bool state);
      const KLExtension* loadExtension(const char * name, const char * jsonContent, uint32_t numKlFiles, const char ** klContent);
      const KLExtension* loadExtension(const char * jsonFilePath);
      void loadAllExtensionsInFolder(const char * extensionFolder, bool parseExtensions = true);
      bool loadAllExtensionsFromExtsPath(bool parseExtensions = true);
      const KLExtension* loadExtensionFromExtsPath(const char * name);
      bool removeExtension(const char * name, const char * versionRequirement = "*");
      const KLFile* loadSingleKLFile(const char * klFileName, const char * klContent);

      std::vector<const KLExtension*> getExtensions() const;

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

      // returns the KLType of a given name. If the KLDecl is passed,
      // we will try to resolve this within the same extension, or if the
      // extension doesn't define the type, we'll base it off the requires
      // of th extension.
      virtual const KLType* getKLTypeByName(const char * name, const KLDecl* decl = NULL) const;
      virtual const KLType* getKLTypeByName(const char * name, const KLFile* file) const;
      virtual const KLType* getKLTypeByName(const char * name, const char * extension, const char * versionRequirement = "*") const;

      // returns an extension matching a given version requirement string (for ex: ">1.2.0")
      // if there are multiple resolved versions, we'll use the highest one
      virtual const KLExtension* getExtension(const char * name, const char * versionRequirement = "*") const;
      virtual const KLExtension* getExtension(const KLRequire* require) const;

    protected: 
      uint32_t generateDeclId();

      void registerASTClient(KLASTClient * client);
      void unregisterASTClient(KLASTClient * client);
      void onExtensionLoaded(const KLExtension * extension);
      void onExtensionParsed(const KLExtension * extension);
      void onFileLoaded(const KLFile * file);
      void onFileParsed(const KLFile * file);
      void onASTChanged();

      const KLExtension* loadExtensionFromFolder(const char * name, std::string const &folder);
      const KLExtension* loadExtensionFromFolders(const char * name, std::vector<std::string> const &folders);

    private:
      FabricCore::Client m_client;
      std::vector<const KLExtension*> m_extensions;
      std::vector<KLASTClient*> m_astClients;
      uint32_t m_maxDeclId;
      bool m_isUpdatingASTClients;
      bool m_autoLoadExtensions;

      static KLASTManager * s_manager;
      static uint32_t s_managerRefs;
    };

  };

};

#endif // __ASTWrapper_KLASTManager__
