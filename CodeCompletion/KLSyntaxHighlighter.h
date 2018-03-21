// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __CodeCompletion_KLSyntaxHighlighter__
#define __CodeCompletion_KLSyntaxHighlighter__

#include <FabricCore.h>
#include <string>
#include <vector>
#include <map>

#include <ASTWrapper/KLASTManager.h>
#include <ASTWrapper/KLASTClient.h>
#include <ASTWrapper/KLFile.h>

namespace FabricServices
{

  namespace CodeCompletion
  {

    class KLSyntaxHighlighter : public ASTWrapper::KLASTClient
    {

    public:

      enum Token
      {
        Token_Comment, // maps to FEC_KLTokenType_Comment
        Token_String, // maps to FEC_KLTokenType_String
        Token_Number, // maps to FEC_KLTokenType_Number
        Token_Keyword, // maps to FEC_KLTokenType_Keyword
        Token_Other, // maps to FEC_KLTokenType_Other
        Token_Error, // maps to FEC_KLTokenType_Error
        Token_EOF, // maps to FEC_KLTokenType_EOF
        Token_Type,
        Token_Constant,
        Token_Function,
        Token_Method,
        Token_Highlight,
        Token_NumItems
      };

      struct Format
      {
        Token token;
        uint32_t start;
        uint32_t length;
      };

      KLSyntaxHighlighter(ASTWrapper::KLASTManager * manager);
      virtual ~KLSyntaxHighlighter();

      bool isEnabled() const;
      void setEnabled(bool state);

      static const char * getTokenName(Token token);

      // highlighting
      virtual const std::vector<Format> & getHighlightFormats(const std::string & text) const;
      virtual std::string getHighlightedText(const std::string & text) const;

      // error reporting
      virtual void reportError(uint32_t start, uint32_t length);
      virtual void clearErrors();

      // highlighting
      virtual void highlight(uint32_t start, uint32_t length);
      virtual void clearHighlighting();

      virtual void onFileParsed(const ASTWrapper::KLFile * file);

      void initializeBasicTypes(bool force=false);

    private:

      bool m_basicTypesInitialized;
      bool m_enabled;
      mutable std::string m_lastText;
      mutable std::vector<Format> m_lastFormats;
      mutable std::vector<Format> m_errorFormats;
      mutable std::vector<Format> m_highlightFormats;
      mutable std::map<std::string, Token> m_knownTokens;
    };

  };

};


#endif // __CodeCompletion_KLSyntaxHighlighter__
