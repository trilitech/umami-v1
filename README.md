[![Codecov Coverage](https://img.shields.io/codecov/c/gitlab/nomadic-labs/ref-wallet/coverage.svg?style=flat-square)](https://codecov.io/gl/nomadic-labs/ref-wallet)

![umami logo](img/umami-logo.svg "umami-screenshot")


[A powerful new Tezos wallet](https://umamiwallet.com)
===========================================================================

Umami is a cryptocurrency wallet developed by [Nomadic
 Labs](https://www.nomadic-labs.com/) to interact with the [Tezos
 Blockchain](https://tezos.com/). It runs as a desktop electron based app with
 multiple features: dApp interaction, HD Wallet, batch transfer, delegation,
 FA1.2, last operations, etc.

__Umami is in Beta version.__

![umami screenshot](img/screenshot.png "umami-screenshot")

## 📦 Install

Install now from our [release page](https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases)

## 💻 Quick start

### 1) Clone the repository

```
git clone https://gitlab.com/nomadic-labs/umami-wallet/umami/ && cd umami
```

### 2) Requirements:

* [node](https://nodejs.org) installed, version 14 or later.
* [yarn](https://yarnpkg.com) installed.

### 3) Build:

```
yarn
yarn bsb:build
yarn electron:build
```
The Electron binary is located in the `dist` folder.

### 4) (Optional) Run for development

```
yarn electron:dev
```

## ℹ️ Support

- [open an issue](https://gitlab.com/nomadic-labs/umami-wallet/testruns/-/issues/new?issuable_template=support)
- `#umami` channel on [tezos slack](https://tezos-dev.slack.com)
- [email](mailto:incoming+nomadic-labs-umami-wallet-umami-20392089-issue-@incoming.gitlab.com)
