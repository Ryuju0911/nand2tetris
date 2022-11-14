#include <fstream>
#include <iostream>
#include <string>

#include "compilation_engine.h"
#include "tokenizer.h"
#include "utils.h"

using std::string;

int main (int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "The arguments are not valid.\n";
    exit(1);
  }

  // Create the output file.
  string output_file = utils::GetFileNameFromPath(argv[1]) + ".xml";

  // Start parsing.
  compiler::CompilationEngine parser(argv[1], output_file);
  parser.Parse();

  return 0;
}
