[[_TOC_]]

# Compatibility Table - Prescribed versions

We list here the prescribed versions recommended for each of the tools needed to run the Umami project.

## Front-end
We focus here on the compatibility between Umami client version and the front-end Mezos.

| mezos          | Umami | Release |(comment) |
|---             |---    |---     |---   |
| TBD (> v1.0.0) | TBD | TBD |  | 
| v1.0.0         | 0.3.4 | 2020-APR-16 | closed-beta release | 


## Back-end
We focus here on the compatibility between the back-end components. 
| tezos-node   | tezos-indexer | mezos          | (Umami)   | Release     |(comment) |
|---           |---            |---             |---        |---          |---   |
| v9.0-rc1     | v9.2.0        | TBD (> v1.0.0) | TBD       | 2021-TBD    | breaking indexer-mezos Compatibility | 
| v9.0-rc1     | v9.1.6        | v1.0.0         | (0.3.4)   | 2020-APR-16 | closed-beta release | 






# Related git repositories

## "back-end" 
### Tezos Codebase 
Git repo with umami specific branches
* tezos node : https://gitlab.com/nomadic-labs/tezos/-/tree/master (for tezos-node)

###  Tezos Indexer
Indexer connects to the node and store the 
  * https://gitlab.com/nomadic-labs/tezos-indexer

###  Mezos
*  https://gitlab.com/nomadic-labs/mezos
 
## Umami 
* Umami (Front-end client - Electron app) : https://gitlab.com/nomadic-labs/umami

## Tezos Codebase with specific code for front-end
* Tezos JS packages : https://gitlab.com/nomadic-labs/sdk-js-deps/-/packages


