[[_TOC_]]
# Architecture & Tech Stack

This is high-level only, serves for purposes of planning the tech-stack.

For specifications, refer to [specs](/docs/specs/)

## Wallet  Front-End

```mermaid
graph TD
  RNW[ReactNativeWeb] --> R[React] --> DOM --> E[Electron];
  E --> Window;
  E --> MacOS;
  E --> Linux;
```

### Pros & Cons

| Pros | Cons |
|--|--|
| One single codebase | Stuck in Chrome |
| Integrates well with Reason | Lack of experience internally with Electron |
| Cross OS | |
| Future Proofing portability | |
| Concept proven with Demo (see v0.1.0) | |

## Wallet tezos-client Back-end

### Current

```mermaid
graph TD
  W[Wallet] --> TzCl[tezos-client];
  W --> Ix[Tezos Indexer];
```

### Target

```mermaid
graph TD
  W[Wallet] --> SDK[Tezos SDK];
  W --> Ix[Tezos Indexer];
```


## Wallet indexer Back-end

```mermaid
graph TD
u[umami-client]  
t[tezos-node]
i[tezos-indexer]  
m[mezos]
pg[PostgreSQL DB]

  u --> m
  m --> pg
  m --> t
  i --> t
  i --> pg
```


