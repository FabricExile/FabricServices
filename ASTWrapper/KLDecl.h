// Copyright 2010-2014 Fabric Engine Inc. All rights reserved.

#ifndef __ASTWrapper_KLDecl__
#define __ASTWrapper_KLDecl__

#include <FabricCore.h>

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLASTManager;
    class KLExtension;
    class KLFile;
    class KLLocation;

    typedef const FabricCore::Variant * JSONData;

    enum KLDeclType
    {
      KLDeclType_Alias,
      KLDeclType_CaseStatement,
      KLDeclType_Comment,
      KLDeclType_Commented,
      KLDeclType_CompoundStatement,
      KLDeclType_ConditionalStatement,
      KLDeclType_Constant,
      KLDeclType_CStyleLoopStatement,
      KLDeclType_ExprStatement,
      KLDeclType_Function,
      KLDeclType_Interface,
      KLDeclType_Location,
      KLDeclType_Member,
      KLDeclType_Method,
      KLDeclType_Object,
      KLDeclType_Operator,
      KLDeclType_Parameter,
      KLDeclType_Require,
      KLDeclType_Statement,
      KLDeclType_Struct,
      KLDeclType_SwitchStatement,
      KLDeclType_Type,
      KLDeclType_TypeOp,
      KLDeclType_VarDeclStatement,
      KLDeclType_NumItems
    };

    class KLDecl
    {
    public:

      virtual ~KLDecl();
      virtual bool isInternal() const = 0;
      virtual uint32_t getID() const;
      virtual const KLASTManager* getASTManager() const;
      virtual const KLExtension* getExtension() const;
      virtual const KLFile* getKLFile() const;
      virtual const KLLocation * getLocation() const;

      virtual KLDeclType getDeclType() const = 0;
      virtual bool isOfDeclType(KLDeclType type) const = 0;

    protected:

      KLDecl(const KLFile* klFile, JSONData data);

      uint32_t getArraySize() const;
      const char * getStringArrayElement(uint32_t index) const;
      const char * getStringDictValue(const char * key) const;

      JSONData getArrayElement(uint32_t index) const;
      JSONData getDictValue(const char * key) const;
      JSONData getArrayDictValue(const char * key) const;

    private:

      uint32_t m_id;
      JSONData m_data;
      const KLFile* m_klFile;
      KLLocation * m_location;
    };

  };

};

#endif // __ASTWrapper_KLDecl__
