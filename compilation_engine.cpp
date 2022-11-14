#include "compilation_engine.h"

#include <iostream>
#include <fstream>
#include <set>
#include <string>

#include "tokenizer.h"

using std::cerr;
using std::set;

namespace compiler {

namespace {

const set<string> kOperator {
  "+", "-", "*", "/", "&", "|", "<", ">", "=",
};

} // namespace

// Constructor
CompilationEngine::CompilationEngine(const string &input_file, const string &output_file) : tokenizer_(input_file) {
  input_stream_.open(input_file, std::ios::in);
  if (!input_stream_.is_open()) {
      cerr << "failed to open the input file.\n";
      exit(1);
  }

  output_stream_.open(output_file, std::ios::out);
  if (!output_stream_.is_open()) {
      cerr << "failed to open the output file.\n";
      exit(1);
  }

  Tokenizer tokenizer_(input_file);
  tokenizer_.Advance();
}


// Deconstructor
CompilationEngine::~CompilationEngine() {
  if (input_stream_.is_open()) input_stream_.close();
  if (output_stream_.is_open()) output_stream_.close();
}

// Methods
void CompilationEngine::Parse() {
  while (tokenizer_.HasMoreTokens()) {
    CompileClass();
  }
}

// Compiles class structure
// class: 'class' className '{' classVarDec* subroutineDec* '}'
void CompilationEngine::CompileClass() {
  output_stream_ << "<class>\n";

  WriteToken(); // 'class'
  WriteToken(); // className
  WriteToken(); // '{'
  
  while (tokenizer_.GetCurrentToken() == "static" || tokenizer_.GetCurrentToken() == "field") {
    // Compile classVarDec
    CompileClassVarDec();
  }

  while (tokenizer_.GetCurrentToken() == "constructor" || tokenizer_.GetCurrentToken() == "function" ||
      tokenizer_.GetCurrentToken() == "method") {
    // Compile subroutineDec
    CompileSubroutineDec();
  }
  
  WriteToken(); // '}'

  output_stream_ << "</class>\n";
}

// Compiles classVarDec
// classVarDec: ('static' | 'field') type varName (',' varName)* ';'
void CompilationEngine::CompileClassVarDec() {
  output_stream_ << "<classVarDec>\n";

  WriteToken(); // 'static' | 'field'
  WriteToken(); // 'int' | 'char' | 'boolean' | className

  // varName or ','
  while (tokenizer_.GetCurrentToken() != ";") {
      WriteToken();
  }
  WriteToken(); // ';'
  output_stream_ << "</classVarDec>\n";
}

// Compiles subroutineDec
// subroutineDec: ('constructor' | 'function' | 'method') ('void' | type) subroutineName
// '(' parameterList ')' subroutineBody
void CompilationEngine::CompileSubroutineDec() {
  output_stream_ << "<subroutineDec>\n";

  WriteToken(); // 'constructor' | 'function' | 'method'
  WriteToken(); // 'void' | type
  WriteToken(); // subroutineName
  WriteToken(); // '('

  CompileParameterList(); // parameterList

  WriteToken(); // ')'

  CompileSubroutineBody(); // subroutineBody

  output_stream_ << "</subroutineDec>\n";
}

// Compiles parameterList
// parameterList: ((type varName) (',' type varName)*)?
void CompilationEngine::CompileParameterList() {
  output_stream_ << "<parameterList>\n";

  // type, varName or ','
  while (tokenizer_.GetCurrentToken() != ")") {
    WriteToken(); 
  }

  output_stream_ << "</parameterList>\n";
}

// Compiles subroutineBody
// subroutineBody: '{' varDec* statements '}'
void CompilationEngine::CompileSubroutineBody() {
  output_stream_ << "<subroutineBody>\n";

  WriteToken(); // '{'

  while (tokenizer_.GetCurrentToken() == "var") {
    CompileVarDec();
  }
  CompileStatements();

  WriteToken(); // '}'

  output_stream_ << "</subroutineBody>\n";
}

// Compiles varDec
// varDec: 'var' type varName (',' varName)* ';'
void CompilationEngine::CompileVarDec() {
  output_stream_ << "<varDec>\n";

  WriteToken(); // var

  // type, varName or ','
  while (tokenizer_.GetCurrentToken() != ";") {
    WriteToken();
  }
  WriteToken(); // ';'

  output_stream_ << "</varDec>\n";
}

// Compiles statements
// statements: statement*
// There are five kinds of statements, which are letStatement, ifStatement,
// whileStatement, doStatement, and returnStatement.
void CompilationEngine::CompileStatements() {
  output_stream_ << "<statements>\n";

  while (true) {
    if (tokenizer_.GetCurrentToken() == "let") CompileLet();
    else if (tokenizer_.GetCurrentToken() == "if") CompileIf();
    else if (tokenizer_.GetCurrentToken() == "while") CompileWhile();
    else if (tokenizer_.GetCurrentToken() == "do") CompileDo();
    else if (tokenizer_.GetCurrentToken() == "return") CompilerReturn();
    else break;
  }

  output_stream_ << "</statements>\n";
}

// Compiles letStatement
// letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
void CompilationEngine::CompileLet() {
  assert(tokenizer_.GetCurrentToken() == "let");
  output_stream_ << "<letStatement>\n";

  WriteToken(); // 'let'
  WriteToken(); // varName

  if (tokenizer_.GetCurrentToken() == "[") {
    WriteToken(); // '['
    CompileExpression();
    WriteToken(); // ']'
  }

  WriteToken(); // '='
  CompileExpression();
  WriteToken(); // ';'

  output_stream_ << "</letStatement>\n";
}

// Compiles ifStatement
// ifStatement: 'if' '(' expression ')' '{' statements '}'
// ('else' '{' statements '}')?
void CompilationEngine::CompileIf() {
  assert(tokenizer_.GetCurrentToken() == "if");
  output_stream_ << "<ifStatement>\n";

  // 'if' '(' expression ')' '{' statements '}'
  WriteToken(); // 'if'
  WriteToken(); // '('
  CompileExpression();
  WriteToken(); // ')'
  WriteToken(); // '{'
  CompileStatements();
  WriteToken(); // '}'

  // ('else' '{' statements '}')?
  if (tokenizer_.GetCurrentToken() == "else") {
    WriteToken(); // 'else'
    WriteToken(); // '{'
    CompileStatements();
    WriteToken(); // '}'
  }

  output_stream_ << "</ifStatement>\n";
}

// Compiles whileStatement
// whileStatement: 'while' '(' expression ')' '{' statements '}'
void CompilationEngine::CompileWhile() {
  assert(tokenizer_.GetCurrentToken() == "while");
  output_stream_ << "<whileStatement>\n";

  WriteToken(); // 'while'
  WriteToken(); // '('
  CompileExpression();
  WriteToken(); // ')'
  WriteToken(); // '{'
  CompileStatements();
  WriteToken(); // '}'

  output_stream_ << "</whileStatement>\n";
}

// Compiles doStatement
// doStatement: 'do' subroutineCall ';'
// subroutinCall: subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName
// '(' expressionList ')'
void CompilationEngine::CompileDo() {
  assert(tokenizer_.GetCurrentToken() == "do");
  output_stream_ << "<doStatement>\n";

  WriteToken(); // 'do'

  // subroutineCall
  WriteToken(); // subroutineName | (className | varName)
  if (tokenizer_.GetCurrentToken() == "(") {
    // '(' expressionList ')'
    WriteToken(); // '('
    CompileExpressionList(); // expressionList
    WriteToken(); // ')'
  } else {
    // '.' subroutineName '(' expressionList ')'
    WriteToken(); // '.'
    WriteToken(); // subroutineName
    WriteToken(); // '('
    CompileExpressionList(); // expressionList
    WriteToken(); // ')'
  }

  WriteToken(); // ';'

  output_stream_ << "</doStatement>\n";
}

// Compiles returnStatement
// returnStatement: 'return' expression? ';'
void CompilationEngine::CompilerReturn() {
  assert(tokenizer_.GetCurrentToken() == "return");
  output_stream_ << "<returnStatement>\n";
  
  WriteToken(); // 'return'
  if (tokenizer_.GetCurrentToken() != ";") CompileExpression(); // expression
  WriteToken(); // ';'

  output_stream_ << "</returnStatement>\n";
}

// Compiles expression
// expression: term (op term)*
// expression ends when current token is ']', ';', ')', or ','.
void CompilationEngine::CompileExpression() {
  output_stream_ << "<expression>\n";

  CompileTerm(); // term

  while (kOperator.find(tokenizer_.GetCurrentToken()) != kOperator.end()) {
      WriteToken(); // op
      CompileTerm(); // term
  }

  output_stream_ << "</expression>\n";
}

// Compiles term
// term: integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' |
//       subroutineCall | '{' expression '}' | unaryOp term
// subroutineCall: subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName
//                 '(' expressionList ')'
void CompilationEngine::CompileTerm() {
  output_stream_ << "<term>\n";

  switch (tokenizer_.GetTokenType()) {
    case kIntConstType: {
      WriteToken(); // integerConstant
      break;
    }
    
    case kStringConstType: {
      WriteToken(); // stringConstant
      break;
    }
    case kReservedWordType: {
      WriteToken(); // keywordConstant
      break;
    }
    
    case kIdentifierType: {
      // varName | varName '[' expression ']' | subroutineCall
      WriteToken(); // varName, subroutineName, or (className | varName)

      if (tokenizer_.GetCurrentToken() == "(") {
        // subroutineName '(' expressionList ')'
        WriteToken(); // '('
        CompileExpressionList(); // expressionList
        WriteToken(); // ')'
      } else if (tokenizer_.GetCurrentToken() == ".") {
        // (className | varName) '.' subroutineName '(' expressionList ')'
        WriteToken(); // '.'
        WriteToken(); // subroutineName
        WriteToken(); // '('
        CompileExpressionList(); // expressionList
        WriteToken(); // ')'
      } else if (tokenizer_.GetCurrentToken() == "[") {
        // varName '[' expression ']'
        WriteToken(); // '['
        CompileExpression(); // expression
        WriteToken(); // ']'
      }
      break;
    }

    case kSymbolType: {
      if (tokenizer_.GetCurrentToken() == "(") {
        // '(' expression ')'
        WriteToken(); // '('
        CompileExpression(); // expression
        WriteToken(); // ')'
      } else if (tokenizer_.GetCurrentToken() == "-" || tokenizer_.GetCurrentToken() == "~") {
        // unaryOp term
        WriteToken(); // unaryOp
        CompileTerm(); // term
      }
      break;
    }

    default:
      break;
  }
  output_stream_ << "</term>\n";
}

// Compiles expressionList
// expressionList: (expression (',' expression)* )?4
// Note that expressionList is always ends with ')'
void CompilationEngine::CompileExpressionList() {
  output_stream_ << "<expressionList>\n";

  if (tokenizer_.GetCurrentToken() != ")") {
    CompileExpression(); // expression
    while (tokenizer_.GetCurrentToken() == ",") {
      WriteToken(); // ','
      CompileExpression(); // expression
    }
  }

  output_stream_ << "</expressionList>\n";
}

// Writes token into the output file with xml tag
void CompilationEngine::WriteToken() {
  if (tokenizer_.GetTokenType() == compiler::kReservedWordType) {
    output_stream_ << "<keyword> " << tokenizer_.GetReservedWord() << " </keyword>\n";
  } else if (tokenizer_.GetTokenType() == compiler::kSymbolType) {
    output_stream_ << "<symbol> " << tokenizer_.GetSymbol() << " </symbol>\n";
  } else if (tokenizer_.GetTokenType() == compiler::kIdentifierType) {
    output_stream_ << "<identifier> " << tokenizer_.GetIdentifier() << " </identifier>\n";
  } else if (tokenizer_.GetTokenType() == compiler::kStringConstType) {
    output_stream_ << "<stringConstant> " << tokenizer_.GetStringVal() << " </stringConstant>\n";
  } else if (tokenizer_.GetTokenType() == compiler::kIntConstType) {
    output_stream_ << "<integerConstant> " << tokenizer_.GetIntVal() << " </integerConstant>\n";
  }

  // Move on to the next token.
  tokenizer_.Advance();
}
} // namespace compiler