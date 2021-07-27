#include "eval.hh"
#include "globals.hh"
#include "store-api.hh"


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  using namespace nix;

  initGC();
  Strings attrPaths = {""};


  static EvalState state(Strings(), openStore("dummy://"));
  state.repair = NoRepair;

  Bindings * autoArgs = state.allocBindings(0);

  // Parse input data into an expression.
  std::string expressionString = std::string((char*)Data, Size);
  Expr * expression;
  try {
    expression = state.parseExprFromString(expressionString, absPath("."));
  }
  // Some errors are legitimate, so we want to gracefully return when they are raised.
  catch (const ParseError &) {
    return 0;
  }
  catch (const UndefinedVarError &) {
    return 0;
  }
  catch (const TypeError &) {
    return 0;
  }
  catch (const Unsupported &) {
    return 0;
  }

  return 0;
}
