#include <fstream>
#include <iostream>
#include <string>

#include "tokenizer.h"
#include "utils.h"

using std::string;

int main (int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "The arguments are not valid.\n";
    exit(1);
  }

  // Create and open the output file.
  string output_file = utils::GetFileNameFromPath(argv[1]) + ".xml";
  ofstream output_stream(output_file, std::ios::out);
  if (!output_stream) {
    std::cerr << "failed to open the output file.\n";
    exit(1);
  }

  // Start tokenizing.
  compiler::Tokenizer tokenizer(argv[1]);
  output_stream << "<tokens>\n";

  while (tokenizer.HasMoreTokens()) {
    // Write the token type and the current code into the xml file.
    if (tokenizer.GetTokenType() == compiler::kReservedWordType) {
      output_stream << "<keyword> " << tokenizer.GetReservedWord() << " </keyword>\n";
    } else if (tokenizer.GetTokenType() == compiler::kSymbolType) {
      output_stream << "<symbol> " << tokenizer.GetSymbol() << " </symbol>\n";
    } else if (tokenizer.GetTokenType() == compiler::kIdentifierType) {
      output_stream << "<identifier> " << tokenizer.GetIdentifier() << " </identifier>\n";
    } else if (tokenizer.GetTokenType() == compiler::kStringConstType) {
      output_stream << "<stringConstant> " << tokenizer.GetStringVal() << " </stringConstant>\n";
    } else if (tokenizer.GetTokenType() == compiler::kIntConstType) {
      output_stream << "<integerConstant> " << tokenizer.GetIntVal() << " </integerConstant>\n";
    }

    // Move on to the next token.
    tokenizer.Advance();
  }

  output_stream << "</tokens>\n";

  output_stream.close();
}
