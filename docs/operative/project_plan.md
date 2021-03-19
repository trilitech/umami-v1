# Ref-Wallet - Project Roadmap

This is the **Project** Roadmap, which plans the progress of the development efforts of Umami.

## Projected Milestones:

 * Alpha version: Nov 2020 (internal only--usable with basic feature set)
     * Accounts
     * Aliases
     * Transactions
     * Operation History
     * Delegation
 * Beta release: Mar 2021 (limited release--more features, some left in the backlog: TBD)
     * Batch Transactions
     * FA1.2 Tokens
 * v1.0 release: Jun 2021 (public release--stable)
     * FA2
     * Remote Signing (TZIP-10)
     * Hardware wallet support
     * DirectAuth
 * v1.x maintenance phase
 * v2.0 release: Q4 2021 (public release--no backlog)
     * Contracts
     * Sapling
     * Multisig

 **Note**:

  * dates are pessimistic/at-latest, given best-available-data
  * dates are subject to a degree of flux if/when feature/scope is expanded
  * roadmap will be amended if extenuating circumstances occur

## Gantt Chart

```mermaid
gantt
    title Release Overview
    Alpha Release                                     :done,   2020-07-19, 2020-12-01
    Beta Release                                      :active, 2020-12-01, 2021-03-05
    Release - v 1.0                                   :active, 2021-03-05, 2021-06-01
    Maintenance - v 1.x                               :active, 2021-06-01, 2021-09-01
    Upgrade - v 2.0                                   :active, 2021-06-01, 2021-09-01
```

```mermaid
gantt
    title Sprint Overview
    section Project Initiation
    Cortez export                                     :done  , 2020-07-01 , 30d
    Project Initiation                                :done  , 2020-06-01, 2020-07-13
    section Team Hiring
    Team Hiring                                       :done  , 2020-06-11, 2020-09-01
    Onboarding New Members                            :done  , 2020-10-16, 2020-11-01
    section Sprint 1 - PoC
    Sprint 1 - PoC                                    :done  , 2020-07-19, 2020-07-31
    section Sprint 2 - Simple Wallet
    Sprint 2 - Simple Wallet                          :done  , 2020-08-03, 2020-08-14
    section Sprint 3 - Indexer Refactoring
    Sprint 3 - Indexer Refactoring                    :done  , 2020-08-17, 2020-08-28
    section Sprint 4 - Delegation
    Sprint 4 - Delegation                             :done  , 2020-08-31, 2020-09-18
    section Sprint 5 - CI & Testing
    Sprint 5 - CI & Testing                           :done  , 2020-09-21, 2020-10-02
    section Sprint 6 - UX Integration
    Sprint 6 - UX Integration                         :done  , 2020-10-05, 2020-10-16
    section Sprint 7 - Alpha Candidate
    Sprint 7 - Alpha Candidate                        :done  , 2020-10-19, 2020-10-30
    section Sprint 8 - Alpha
    Sprint 8 - Alpha                                  :done  , 2020-11-02, 2020-11-13
    section Sprint 9 - Refactoring
    Sprint 9 - Refactoring                            :done  , 2020-11-15, 2020-11-27
    section Sprint 10 - Tokens FA1.2
    Sprint 10 - Tokens                                :done  , 2020-12-01, 2020-12-11
    section Sprint 11 - Batch Txs
    Sprint 11 - Batch Txs                             :done  , 2020-12-15, 2021-01-08
    section Sprint 12 - Token Batching
    Sprint 12 - Token Batching                        :done  , 2021-01-11, 2021-01-22
    section Sprint 13 - Key Management
    Sprint 13 - Key Management                        :done  , 2021-01-25, 2021-02-05
    section Sprint 14 - SDK Integ
    Sprint 14 - SDK Integ                             :done  , 2021-02-08, 2021-02-19
    section Beta Candidate
    Sprint 15 - SDK Integ & Headstart on FA2          :done. , 2021-02-22, 2021-03-05
    section Beta Candidate
    Sprint 16 - SDK Integ & Headstart on FA2          :active, 2021-03-08, 2021-03-19
    section Beta Candidate
    Sprint 17 - Beta Candidate                        :active, 2021-03-22, 2021-04-05
```

