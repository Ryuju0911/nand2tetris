#include <fstream>
#include <iostream>
#include <string>

#include "tokenizer.h"
#include "utils.h"

int main (int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "The arguments are not valid.\n";
    exit(1);
  }

  // Create and open the output file.
  std::string output_file = utils::GetFileNameFromPath(argv[1]) + ".xml";
  std::ofstream output_stream(output_file, std::ios::out);
  if (!output_stream) {
    std::cerr << "failed to open the output file.\n";
    exit(1);
  }

  // Start tokenizing.
  tokenizer::Tokenizer tokenizer(argv[1]);
  output_stream << "<tokens>\n";

  while (tokenizer.HasMoreTokens()) {
    // Check the type of the current token.
    tokenizer.SetTokenType();

    // Write the token type and the current code into the xml file.
    if (tokenizer.GetTokenType() == tokenizer::kReservedWordType) {
      output_stream << "<keyword> " << tokenizer.GetReservedWord() << " </keyword>\n";
    } else if (tokenizer.GetTokenType() == tokenizer::kSymbolType) {
      output_stream << "<symbol> " << tokenizer.GetSymbol() << " </symbol>\n";
    } else if (tokenizer.GetTokenType() == tokenizer::kIdentifierType) {
      output_stream << "<identifier> " << tokenizer.GetIdentifier() << " </identifier>\n";
    } else if (tokenizer.GetTokenType() == tokenizer::kStringConstType) {
      output_stream << "<stringConstant> " << tokenizer.GetStringVal() << " </stringConstant>\n";
    } else if (tokenizer.GetTokenType() == tokenizer::kIntConstType) {
      output_stream << "<integerConstant> " << tokenizer.GetIntVal() << " </integerConstant>\n";
    }

    // Move on to the next token.
    tokenizer.Advance();
  }

  output_stream << "</tokens>\n";

  output_stream.close();
}