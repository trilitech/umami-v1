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
```

