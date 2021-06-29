with import <nixpkgs> {};

let
  inherit (lib) attrValues mapAttrs';

  getHostname = path: lib.lists.last (lib.splitString "/" path);
  getConfiguration = path: "${toString ./.}/${path}/configuration.nix";

  hosts = {

    x86_64-linux = [
      "rsn/goat"
      "rsn/hedgehog"
    ];
  };

  genAttrs' = func: values: builtins.listToAttrs (map func values);
in
attrValues (
  mapAttrs'
    (system: paths:
      genAttrs'
        (path: {
          name = getHostname path;
          value = {
            inherit system;
            channelName = "nixpkgs";
            modules = [ (getConfiguration path) ];
          };
        })
        paths
    )
    hosts
)
