#include <fstream>

#include "YAML.h"
#include <antlr4-runtime.h>

#include <kdb.hpp>

#include "ErrorListener.hpp"
#include "Listener.hpp"
#include "YAMLLexer.hpp"

using namespace antlr;
using namespace antlr4;
using namespace ckdb;
using namespace std;

using CppKey = kdb::Key;

int main(int argc, char const *argv[]) {

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return EXIT_FAILURE;
  }

  ifstream file{argv[1]};
  if (!file.is_open()) {
    cerr << "Unable to open file “" << argv[1] << "”" << endl;
    return EXIT_FAILURE;
  }

  stringstream text;
  text << file.rdbuf();
  cout << "— Input ———————" << endl << endl << text.str() << endl;
  ANTLRInputStream input(text.str());
  YAMLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  cout << "— Tokens ——————" << endl << endl;
  for (auto token : tokens.getTokens()) {
    cout << token->toString() << endl;
  }

  YAML parser(&tokens);
  ErrorListener errorListener{};
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);

  tree::ParseTreeWalker walker{};
  KeyListener listener{keyNew("user", KEY_END, "", KEY_VALUE)};

  antlr4::tree::ParseTree *tree = parser.yaml();
  cout << endl << "— Tree ——————" << endl << endl;
  cout << tree->toStringTree() << endl << endl;

  cout << "— Output ————" << endl << endl;
  walker.walk(&listener, tree);
  for (auto key : listener.keySet()) {
    cout << key.getName() << ": " << key.getString() << endl;
  }
  return EXIT_SUCCESS;
}
