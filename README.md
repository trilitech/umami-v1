[![Codecov Coverage](https://img.shields.io/codecov/c/gitlab/nomadic-labs/ref-wallet/coverage.svg?style=flat-square)](https://codecov.io/gl/nomadic-labs/ref-wallet/)

Umami Wallet
===========================================================================

Reference cryptocurrency wallet to interact with Tezos Blockchain. 
Umami is a desktop electron based app with various features: 

- manage multiple accounts with XTZ
- tranfers, delegates
- token (FA1.2) interactions
- last operations

__Umami is still in ALPHA version. Please do not use it with your Mainnet keys.__

# Install from sources

### Requirements

* [node](https://nodejs.org)
* [yarn](https://yarnpkg.com)
* [tezos-client](https://gitlab.com/tezos/tezos)
  It must be accessible from PATH where electron starts.

### Compile Tezos Client (branch umami@dry_run_no_pass+fa1.2)

To compile Tezos you need rust.

You can use the provided script get-and-compile-tezos.sh, it will get and compile the branch alex@dry_run_no_pass of Tezos.

### Scripts

* install Js dependancies

`yarn install`

* build code

`yarn bsb:build`

* start the electron server with hot refresh

`yarn electron:dev`

* Production script

`npm run electron:build`

Builds the Electron app package for production to the `dist` folder.
