Tezos Client GUI
===========================================================================

## Requirements

* [node](https://nodejs.org)
* [yarn](https://yarnpkg.com)

* [tezos-client](https://gitlab.com/tezos/tezos)
It must be accessible from PATH where electron starts.
and at least version 8

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
