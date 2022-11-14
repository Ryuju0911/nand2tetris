#ifndef COMPILER_ENGINE_H_
#define COMPILER_ENGINE_H_

#include <fstream>
#include <string>

#include "tokenizer.h"

using std::ifstream;
using std::ofstream;
using std::string;

namespace compiler {

class CompilationEngine {
  public:
    // Constructor
    CompilationEngine(const string &input_file, const string &output_file);

    // Deconstructor
    ~CompilationEngine();

    // Methods
    void Parse();
    void CompileClass();
    void CompileClassVarDec();
    void CompileSubroutineDec();
    void CompileParameterList();
    void CompileSubroutineBody();
    void CompileVarDec();
    void CompileStatements();
    void CompileLet();
    void CompileIf();
    void CompileWhile();
    void CompileDo();
    void CompilerReturn();
    void CompileExpression();
    void CompileTerm();
    void CompileExpressionList();

    void WriteToken();

  private:
    ifstream input_stream_;
    ofstream output_stream_;
    Tokenizer tokenizer_;
};
} // namespace compilation_engine

#endif