[](./coverage/badge.svg)

Tezos Client GUI
===========================================================================

## Requirements

* [node](https://nodejs.org)
* [yarn](https://yarnpkg.com)
* [tezos-client](https://gitlab.com/tezos/tezos) (branch alex@dry_run_no_pass)
  It must be accessible from PATH where electron starts.

## Compile Tezos Client (branch alex@dry_run_no_pass)

To compile Tezos you need rust.

You can use the provided script get-and-compile-tezos.sh, it will get and compile the branch alex@dry_run_no_pass of Tezos.

## Development scripts

* install dependancies

`yarn install`

* build the reason code

`yarn bsb:build`

* or start autocompilation server and file watcher

`yarn bsb:start`

* start the electron server with hot refresh

`yarn electron:dev`

* Production script

`npm run electron:build`

Builds the Electron app package for production to the `dist` folder.
