// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __CodeCompletion_KLCodeAssistant__
#define __CodeCompletion_KLCodeAssistant__

#include "KLSyntaxHighlighter.h"
#include "KLVariable.h"
#include <ASTWrapper/KLASTManager.h>
#include <ASTWrapper/KLASTClient.h>
#include <map>

namespace FabricServices
{

  namespace CodeCompletion
  {

    class KLCodeAssistant : public ASTWrapper::KLASTClient
    {
    public:

      KLCodeAssistant(ASTWrapper::KLASTManager * manager = NULL);
      KLCodeAssistant(KLSyntaxHighlighter * highlighter);
      virtual ~KLCodeAssistant();

      virtual bool setASTManager(ASTWrapper::KLASTManager * manager);

      KLSyntaxHighlighter * getHighlighter();
      const ASTWrapper::KLFile * getKLFile();
      std::vector<const ASTWrapper::KLError*> getKLErrors();

      bool updateCurrentKLFile(const ASTWrapper::KLFile * file);
      bool updateCurrentCodeAndFile(const std::string & code, const std::string & fileName, bool updateAST = true);

      void lineAndColumnToCursor(uint32_t line, uint32_t column, uint32_t & cursor) const;
      void cursorToLineAndColumn(uint32_t cursor,  uint32_t & line, uint32_t & column) const;

      bool isCursorInsideCommentOrString(uint32_t cursor) const;
      bool isCursorInsideCommentOrString(uint32_t line, uint32_t column) const;

      const ASTWrapper::KLStmt * getStatementAtCursor(uint32_t cursor) const;
      const ASTWrapper::KLStmt * getStatementAtCursor(uint32_t line, uint32_t column) const;
      std::string getCodeForStatement(const ASTWrapper::KLStmt * statement) const;

      std::string getWordAtCursor(uint32_t cursor, bool ignoreParentheses) const;
      std::string getWordAtCursor(uint32_t line, uint32_t column, bool ignoreParentheses) const;
      std::string getCharAtCursor(uint32_t cursor) const;
      std::string getCharAtCursor(uint32_t line, uint32_t column) const;

      std::vector<KLVariable> getVariablesAtCursor(uint32_t cursor) const;
      std::vector<KLVariable> getVariablesAtCursor(uint32_t line, uint32_t column) const;

      const ASTWrapper::KLDecl * getDeclAtCursor(uint32_t cursor) const;
      const ASTWrapper::KLDecl * getDeclAtCursor(uint32_t line, uint32_t column) const;
      const ASTWrapper::KLType * getTypeAtCursor(uint32_t cursor, bool ignoreInvalidMethod = false) const;
      const ASTWrapper::KLType * getTypeAtCursor(uint32_t line, uint32_t column, bool ignoreInvalidMethod = false) const;

    private:

      void init();
      const char * resolveAliases(const char * name) const;

      KLSyntaxHighlighter * m_highlighter;
      bool m_owningHighlighter;

      std::string m_code;
      std::vector<std::string> m_lines;
      std::string m_fileName;
      const ASTWrapper::KLFile * m_file;
    };

  };

};


#endif // __CodeCompletion_KLCodeAssistant__
