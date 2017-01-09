// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __ASTWrapper_KLTypeOp__
#define __ASTWrapper_KLTypeOp__

#include "KLFunction.h"

#include <string>

namespace FabricServices
{

  namespace ASTWrapper
  {

    class KLTypeOp : public KLFunction
    {
      friend class KLFile;

    public:

      enum OpType
      {
        OpType_Equal,
        OpType_NotEqual,
        OpType_LessThan,
        OpType_GreaterThan,
        OpType_LessEquals,
        OpType_GreaterEquals,
        OpType_Assign,
        OpType_Add,
        OpType_Sub,
        OpType_Mul,
        OpType_Div,
        OpType_Neg,
        OpType_Pos,

        OpType_NumElements
      };

      virtual ~KLTypeOp();

      virtual KLDeclType getDeclType() const;
      virtual bool isOfDeclType(KLDeclType type) const;


      const std::string & getName() const;
      const std::string & getReturnType() const;
      uint32_t getParameterCount() const;
      const KLParameter * getParameter(uint32_t index) const;

      virtual bool isMethod() const;
      virtual std::string getPrefix() const; // the prefix of the function *may* be something like 'Vec3.'
      virtual std::string getSuffix() const; // the suffix can be '!' or '?'
      virtual const char * getKLType() const;
      virtual std::string getKLCode(bool includeReturnType = true, bool includeKeyWord = true, bool includePrefix = true, bool includeName = true) const;
      virtual std::string getLabel() const;

      virtual OpType getOpType() const;
      virtual bool isUnary() const;
      virtual std::string getLhs() const;
      virtual std::string getRhs() const;

    protected:

      KLTypeOp(const KLFile* klFile, JSONData data);

    private:

      OpType m_op;
      mutable std::string m_name;
      mutable std::string m_returnType;
      mutable std::string m_label;
      std::string m_lhs;
      std::string m_rhs;
      bool m_isUnary;
    };

  };

};


#endif // __ASTWrapper_KLTypeOp__
