#ifndef COMPILER_TOKENIZER_H_
#define COMPILER_TOKENIZER_H_

#include <fstream>
#include <set>
#include <string>

using std::ifstream;
using std::ofstream;
using std::set;
using std::string;

namespace compiler {

enum TokenType {
  kReservedWordType, kSymbolType, kIdentifierType, kIntConstType, kStringConstType,
};

enum ReservedWord {
  kClass, kMethod, kFunction, kConstructor, kInt, kBoolean, kChar, kVoid, kVar,
  kStatic, kField, kLet, kDo, kIf, kElse, kWhile, kReturen, kTrue, kFalse, kNull, kThis,
};

class Tokenizer {
  public:
    // Constructor
    Tokenizer(const string &input_file);

    // Deconstructor
    ~Tokenizer();

    // Methods
    string GetCurrentToken();
    string GetNextToken();
    TokenType GetTokenType();
    bool HasMoreTokens();
    void Advance();
    void SetTokenType();
    string GetReservedWord();
    char GetSymbol();
    string GetIdentifier();
    int GetIntVal();
    string GetStringVal();
    void WriteToken();

  private:
    // Members
    ifstream input_stream_;
    string current_token_;
    string next_token_;
    TokenType current_token_type_;
};

} // namespace tokenizer

#endif // COMPILER_TOKENIZER_H_