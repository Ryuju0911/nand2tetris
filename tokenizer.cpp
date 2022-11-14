#include "tokenizer.h"

#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include "utils.h"

using std::set;
using std::string;

namespace compiler {

namespace {

const set<string> kKeywords {
  "class", "constructor", "function", "method", "field", "static", "var", "int",
  "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if",
  "else", "while", "return"
};

const set<char> kSymbols {
  {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'}
};

} // namespace

// Constructor
// Opens the input file and gets ready to tokenize it
Tokenizer::Tokenizer(const string &input_file) {
  input_stream_.open(input_file, std::ios::in);
  if (!input_stream_.is_open()) {
      std::cerr << "failed to open the input file.\n";
      exit(1);
  }

  current_token_ = "";
  next_token_ = "";
  Advance();
};

// Deconstructor
// Closes the input file.
Tokenizer::~Tokenizer() {
  if (input_stream_.is_open()) input_stream_.close();
}

// Methods

string Tokenizer::GetCurrentToken() {
  return current_token_;
}

string Tokenizer::GetNextToken() {
  return next_token_;
}

TokenType Tokenizer::GetTokenType() {
  return current_token_type_;
}

// Checks if there are more tokens in the input
bool Tokenizer::HasMoreTokens() {
  return !input_stream_.eof();
}

// Gets the next token from the input and makes it the current token.
void Tokenizer::Advance() {
  if (current_token_ == "") input_stream_ >> current_token_;

  // handling comments
  while (current_token_ == "//" || current_token_ == "/*" || current_token_ == "/**") {
    // handleing /* comments until closing */ and /** API comments */
    if (current_token_ == "/*" || current_token_ == "/**") {
      int end_of_comment;
      do {
        getline(input_stream_, current_token_);
        end_of_comment = current_token_.find("*/");
      } while (end_of_comment == -1);
      input_stream_ >> current_token_;
    } else {
      // handling // comments to end of line
      getline(input_stream_, current_token_);
      input_stream_ >> current_token_;
    }
  }

  // Check if the current token contains symbol.
  // If so, split the current token.
  for (int i = 0; i < current_token_.size(); i++) {
    if (kSymbols.find(current_token_[i]) != kSymbols.end()) {
      if (i == 0) {
        next_token_ = current_token_.substr(1);
        current_token_ = current_token_[0];
      } else {
        next_token_ = current_token_.substr(i);
        current_token_ = current_token_.substr(0, i);
        break;
      }
    }
  }

  Tokenizer::SetTokenType();
}

// Gets and returns the type of the current token.
void Tokenizer::SetTokenType() {
  if (kKeywords.find(current_token_) != kKeywords.end()) {
    current_token_type_ = kReservedWordType;
  } else if (kSymbols.find(current_token_[0]) != kSymbols.end()) {
    current_token_type_ = kSymbolType;
  } else if (std::isdigit(current_token_[0])) {
    current_token_type_ = kIntConstType;
  } else if (current_token_[0] == '"') {
    current_token_type_ = kStringConstType;
  } else if (!std::isdigit(current_token_[0])) {
    current_token_type_ = kIdentifierType;
  } else {
    std::cerr << "The current token is not valid.\n";
    exit(1);
  }
}

// Returns the reserved word which is the current token.
// Should be called only when GetTokenType() is kReservedWordType.
string Tokenizer::GetReservedWord() {
  assert(GetTokenType() == kReservedWordType);
  string return_reserved_word = current_token_;
  current_token_ = next_token_;
  next_token_ = "";

  return return_reserved_word;
}

// Returns the character which is the current token.
// Should be called only when GetTokenType() is kSymbolType.
char Tokenizer::GetSymbol() {
  assert(GetTokenType() == kSymbolType);
  char return_symbol = current_token_[0];
  current_token_ = next_token_;
  next_token_ = "";

  return return_symbol;
}

// Returns the identifier which is the current token.
// Should be called only when GetTokenType() is kIdentifierType.
string Tokenizer::GetIdentifier() {
  assert(GetTokenType() == kIdentifierType);

  string return_identifier = current_token_;
  current_token_ = next_token_;
  next_token_ = "";

  return return_identifier;
}

// Returns the integer value of the current token.
// Should be called only when GetTokenType() is kIntConstType.
int Tokenizer::GetIntVal() {
  assert(GetTokenType() == kIntConstType);

  string return_int_val = current_token_;
  current_token_ = next_token_;
  next_token_ = "";

  return std::stoi(return_int_val);
}

// Returns the identifier which is the current token.
// Should be called only when GetTokenType() is kStringConstType.
string Tokenizer::GetStringVal() {
  assert(GetTokenType() == kStringConstType);

  char c;
  input_stream_.get(c);
  while (c != '"') {
    current_token_ += c;
    input_stream_.get(c);
  }

  string return_string_val = current_token_.substr(1);
  current_token_ = next_token_;
  next_token_ = "";
  return return_string_val;
}

} // namespace tokenizer
