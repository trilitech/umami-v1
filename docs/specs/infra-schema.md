```mermaid
graph LR
   

  subgraph "Umami Front-end" 
    Node1[Electron app]
    Node3[Tezos CLI or SDK]
  end

subgraph "Tezos"
     Node2[Tezos Node]
  Node5[Tezos Codebase]
end
style Tezos fill:lightblue,stroke:blue,stroke-width:1px


subgraph "(Umami) Back-end"
     Node6[Tezos Indexer - DB]
     Node7[Mezos - web server]
end


  Node5 -- compiles to --> Node3
  Node5 -- compiles to --> Node2

  Node6 -- served on http --> Node7
  Node2 -- indexed --> Node6

  Node2 -- http response --> Node3
  Node7 -- http response --> Node1

```

* lightblue : out of Umami team core responsability (we might have impact)
* yellow : direct Umami team impact

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
## Tezos Codebase :
* tezos-cli / SDK : https://gitlab.com/nomadic-labs/tezos/-/tree/umami@fa1.2+dry_run_no_pass+8.2 (back-end for Umami JS Front-end + by-pass of tezos-cli)
