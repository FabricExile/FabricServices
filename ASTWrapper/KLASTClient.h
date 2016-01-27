// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLASTClient__
#define __ASTWrapper_KLASTClient__

#include "KLDeclContainer.h"
#include "KLLocation.h"
#include "KLExtension.h"

namespace FabricServices
{

  namespace ASTWrapper
  {
    // forward decl
    class KLASTManager;

    class KLASTClient
    {

    public:

      KLASTClient(KLASTManager * manager = NULL);
      virtual ~KLASTClient();

      bool hasASTManager() const;

      virtual const KLASTManager * getASTManager() const;
      virtual KLASTManager * getASTManager();
      virtual bool setASTManager(KLASTManager * manager);

      virtual void onExtensionLoaded(const KLExtension * extension);
      virtual void onExtensionParsed(const KLExtension * extension);
      virtual void onFileLoaded(const KLFile * file);
      virtual void onFileParsed(const KLFile * file);
      virtual void onASTChanged();

    private:
      KLASTManager * m_manager;
    };

  };

};

#endif // __ASTWrapper_KLASTClient__
