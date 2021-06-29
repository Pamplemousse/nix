#include "attr-path.hh"
#include "eval.hh"
#include "filetransfer.hh"
#include "get-drvs.hh"
#include "globals.hh"
#include "local-fs-store.hh"
#include "store-api.hh"
#include "types.hh"
#include "url.hh"


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  using namespace nix;

  // Initialize some stuff;
  // See `src/nix-instantiate/nix-instantiate.cc` for more info.

  initGC();
  static Path gcRoot = "";
  static int rootNr = 0;


  // TODO: real opened store VS dummy?
  // auto store = openStore();
  // auto state = std::make_unique<EvalState>(Strings(), store);
  auto state = std::make_unique<EvalState>(Strings(), openStore("dummy://"));
  state->repair = Repair;

  Strings attrPaths = {""};
  Bindings * autoArgs = state->allocBindings(0);

  fileTransferSettings.tries = 1;

  // Format input data into an expression

  std::string expressionString = std::string((char*)Data, Size);
  Expr * expression;
  try {
    expression = state->parseExprFromString(expressionString, absPath("."));
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

  // Parse and evaluate the expression, update the store representation;
  // Adapted from `processExpr` in `src/nix-instantiate/nix-instantiate.cc`.

  Value vRoot;
  try {
    state->eval(expression, vRoot);
  }
  // Some errors are legitimate, so we want to gracefully return when they are raised.
  catch (const BadHash &) {
    return 0;
  }
  catch (const BadStorePath &) {
    return 0;
  }
  catch (const BadURL &) {
    return 0;
  }
  catch (const ExecError &) {
    return 0;
  }
  catch (const UsageError &) {
    return 0;
  }
  catch (const FileTransferError &) {
    return 0;
  }
  catch (const TypeError &) {
    return 0;
  }
  catch (const EvalError &) {
    return 0;
  }
  catch (const SysError &) {
    return 0;
  }
  catch (const UndefinedVarError &) {
    return 0;
  }
  catch (const Unsupported &) {
    return 0;
  }

  state->forceValue(vRoot);

  PathSet context;
  DrvInfos drvs;

  try {
    getDerivations(*state, vRoot, "", *autoArgs, drvs, false);
  }
  // Some errors are legitimate, so we want to gracefully return when they are raised.
  catch (const EvalError &) {
    return 0;
  }
  catch (const FileTransferError &) {
    return 0;
  }
  catch (const SysError &) {
    return 0;
  }
  catch (const UndefinedVarError &) {
    return 0;
  }
  catch (const Unsupported &) {
    return 0;
  }

  for (auto & i : drvs) {
      Path drvPath;
      try {
        drvPath = i.queryDrvPath();
      }
      // Some errors are legitimate, so we want to gracefully return when they are raised.
      catch (const EvalError &) {
        return 0;
      }
      catch (const Unsupported &) {
        return 0;
      }

      string outputName = i.queryOutputName();
      if (outputName == "")
          // Real code throws an error in that case, as it lacks the `outputName` attribute;
          // We exit gracefully instead.
          return 0;
      else {
          Path rootName = absPath(gcRoot);
          if (++rootNr > 1) rootName += "-" + std::to_string(rootNr);
          auto store2 = state->store.dynamic_pointer_cast<LocalFSStore>();
          if (store2)
              drvPath = store2->addPermRoot(store2->parseStorePath(drvPath), rootName);
      }
  }

  return 0;
}
