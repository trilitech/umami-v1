[![Codecov Coverage](https://img.shields.io/codecov/c/gitlab/nomadic-labs/ref-wallet/coverage.svg?style=flat-square)](https://codecov.io/gl/nomadic-labs/ref-wallet/)

Umami Wallet
===========================================================================

Cryptocurrency wallet to interact with Tezos Blockchain.
Umami is a desktop electron based app with multiple features:

- Manage multiple accounts using HD Wallet features
- Register your contacts in you address book
- Tranfers, delegates and batch transfers
- Token (FA1.2) transactions : balance, transfers, batch, etc
- See your last operations

__Umami is still in Beta version.__

## Support

- open an issue
- `#umami` channel on [tezos slack](https://tezos-dev.slack.com)
- [email](mailto:incoming+nomadic-labs-umami-wallet-umami-20392089-issue-@incoming.gitlab.com)

## Install

### binary

- pick your system binary on our [release page](https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases)

### from sources

#### Requirements

* [node](https://nodejs.org)
* [yarn](https://yarnpkg.com)

#### Scripts

##### install Js dependancies

```
yarn install
```

##### build

```
yarn bsb:build
```

* start the electron server with hot refresh

```
yarn electron:dev
```

* Production script

```
yarn run electron:build
```

Builds the Electron app package for production to the `dist` folder.
