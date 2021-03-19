# Ref-Wallet - Project History

## Project Inception

| Date | Step |
|--|--|
| 9 May 2020 | Project was initial conceived conceptually--a Reference Wallet |
| 1 June 2020 | Proposal was drafted by NL and pitched to TF's TAC |

## Original Proposed Roadmap

Extracted from [Original Proposal](/docs/artifacts/plan_for_TAC_01_06_2020.md)

  - Q3 2020:
    - Recruit the new team members
    - Clean stop of Cortez / convert the Cortez team to Reason ML
  - Q4 2020: basic wallet + stable development workflow
  - Q2 2021: reference wallet (beta release)
  
```mermaid
gantt
    title Roadmap for a Reference Wallet
    Recruitment          :crit, a1, 2020-07-01, 90d
    Stop Cortez    :a2, 2020-07-01 , 30d
    ReasonML training :after a2, 60d
    Dev of basic wallet :a4, after a1, 90d
```

## Actuals

```mermaid
gantt
    title Ref-Wallet
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
    Sprint 15 - SDK Integ & Headstart on FA2          :done  , 2021-02-22, 2021-03-05
    section Beta Candidate
    Sprint 16 - SDK Integ & Headstart on FA2          :done  , 2021-03-08, 2021-03-19
```

