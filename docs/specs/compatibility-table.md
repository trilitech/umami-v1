
# Compatibility Table - Prescribed versions

We list here the prescribed versions recommended for each of the tools needed to run the Umami project.

| tezos-node | tezos-indexer | mezos | Umami | (status) |
|---         |---            |---    |---    |---       |
| nomadic-labs/umami@fa1.2+dry_run_no_pass+8.2 | origin/master     | indexer-v9 | origin/develop | branches to build |
| nomadic-labs/pierrick@fa1.2 | (master == v9.1.0)  | indexer-v9-fixes | origin/develop | current dev-api |
| nomadic-labs/pierrick@fa1.2 | (master == v9.1.0)  | indexer-v9-fixes | origin/develop | current qa-api |



# Related git repos

## "back-end" 
### Tezos Codebase :
* tezos node : https://gitlab.com/nomadic-labs/tezos/-/tree/master (tezos-node unmodified)
### Umami Back-end
* (Umami&others) Back-end - Tezos Indexer - DB : https://gitlab.com/nomadic-labs/tezos-indexer
* (Umami&others) Back-end - Mezos - web server : https://gitlab.com/nomadic-labs/mezos

## "front-end"
## Umami Front-end
* Umami Front-end - Electron app : https://gitlab.com/nomadic-labs/umami
* Umami Front-end - Tezos CLI or SDK :
## Tezos Codebase 
* -tezos-cli / SDK : https://gitlab.com/nomadic-labs/tezos/-/tree/umami@fa1.2+dry_run_no_pass+8.2 (back-end for Umami JS Front-end + by-pass of tezos-cli)-
* Tezos JS packages : https://gitlab.com/nomadic-labs/sdk-js-deps/-/packages

