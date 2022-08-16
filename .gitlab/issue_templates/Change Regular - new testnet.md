<!-- "Regular Change" template for new testnets : 
* search&replace 'TESTNET' by the Test-net name (Case-sensitive)
* copy&paste in new issue https://gitlab.com/nomadic-labs/umami-wallet/umami/-/issues/new?issue[title]=Test-net%20Start%20endpoint%20and%20publish:%20TESTNET
* -->
/title Test-net Start endpoint and publish: TESTNET 
/label ~"Umami-stack"  ~"tezos-net::testnet"
/label ~"Priority::2-Medium"
/assign @SamREye @philippewang.info @comeh @sagotch @leoparis89

[[_TOC_]]
## (Regular change) 
<!-- Regular change, to be approved by the CAB before applying. -->

<!-- /confidential -->
<!-- If confidential, explain why -->

<details><summary>Click to expand (step by step gantt chart) <i class="fas fa-chart-gantt" aria-hidden="true"></i> </summary>

```mermaid
gantt

title Deploy TESTNET chain for Umami
todayMarker off

    section Business team
    Biz CAB-Approve           :milestone,  valBIZ, 1h

    section Everybody
    Identify Versions         :crit, versions, before mezos, 1d
    Identify public nodes     :active,      nodes,     1d

	section Operations team
    Ansible templates         :      a1, after versions and valBIZ, 3h
    Ansible inventory         :      a2, after versions and valBIZ, 3h
	deploy .env               :      a3, after a2, 3h
    start env=DEV             :      envDEV, after a3, 1d
    start env=QA              :      envQA,  after a3, 1d
    OPS CAB-Approve           :milestone,      valOPS, after envDEV and envQA and versions, 1h
    start env=PROD            :crit, envPROD, after valDEV, 1d
    monitoring                :      monitoring, after envPROD and versions and nodes, 6h

    section Development team
    Mezos dev                        :active, mezos, 1d
    MR for new option                : MR, after envDEV and envQA, 6h
    Test DEV or QA                   : v2, after envDEV and envQA, 6h
    DEV CAB-Approve                  :milestone, valDEV, after MR and after v2, 1h
    Merge & Release new option        :crit, release, after envPROD and MR, 1d
    CAB close                         :milestone, after release, 1h
   
```

</details>

### Summary
<!-- Outline the issue being faced, and why this required a change !-->

This issue aims at deploying a new tezos-chain: TESTNET.

The different steps allow coordination between Ops and Devs for starting endpoints umami-stack and adding the option in Umami client releases.

#### Step 1 TO-DO : fill-in versions identifiers (DEV + OPS + BIZ)
This table aims at identifying which are the proper versions to use in order to start serving this test net for each of the impacted components.

| Component                  | who should fill this line ?            | **minimal** version                 | **stable** version  |
| ---                        | ---                                    | ---                                 | --                  |
| Umami Client Release       | (@sagotch @leoparis89) @SamREye                   | :warning: (todo : edit this cell) :writing_hand: | :warning: (todo : edit this cell)  :writing_hand:| 
| ~"Umami-stack::mezos"      | (@sagotch @leoparis89) @philippewang.info | `MEZOS_PROTO=TBD` (˟) <br/> :warning: (todo : edit this cell)  :writing_hand:|  `MEZOS_PROTO=TBD`<br/>  :warning: (todo : edit this cell)  :writing_hand:| 
| ~"Umami-stack::indexer"    | @philippewang.info                     | :warning: (todo : edit this cell)  :writing_hand: | :warning: (todo : edit this cell)  :writing_hand:)  | 
| ~"Umami-stack::node" octez | @philippewang.info, @comeh             | :warning: (todo : edit this cell)  :writing_hand: | :warning: (todo : edit this cell)  :writing_hand:  | 

 (˟) needs to be updated until https://gitlab.com/nomadic-labs/mezos/-/issues/20 is developped 


#### Release strategy process 

1. **DEV** and **QA** endpoints : once **minimal** versions (not necesarily stable) of docker images are identified for each ~"Umami-stack" component, we can deploy the TESTNET **DEV** and **QA** endpoints (including monitoring).

2. **PROD** endpoint : once the **stable** versions of docker images are identified for each ~"Umami-stack" component, we can deploy the TESTNET **PROD** endpoint (including monitoring)

3. Once the all  ~"Umami-stack" TESTNET production endpoint is in place, we can release the  ~"App::Frontend" version of Umami client with this ~"tezos-net::testnet" option.


Summary of endpoints to be deployed:
 - [ ] Dev: TESTNET-dev.umamiwallet.com (check when deployed)
 - [ ] QA: TESTNET-qa.umamiwallet.com   (check when deployed)
 - [ ] Prod: TESTNET.umamiwallet.com    (check when deployed)


### Area of the system
<!-- This might only be one part, but may involve multiple sections !-->

* ~"Umami-stack"  : ~"Umami-stack::indexer" ~"Umami-stack::mezos" ~"Umami-stack::node" ~"Umami-stack::monitoring" 
* Umami Client : ~"App::Backend" ~"App::Frontend" 

/label ~"Umami-stack"

### How does this currently work?
<!-- The current process, and any associated business rules !-->

TESTNET is not available.

### What is the desired way of working?
<!-- After the change, what should the process be, and what should the business rules be !-->
* TESTNET is available as an endpoint (Operations team).
* TESTNET is monitored as an endpoint (Operations team).
* TESTNET is available in the Umami application (Development team).

<!-- Success criteria of change application (when relevant, include how to test) -->

* Umami users able to use TESTNET chain.

### Change Procedure
- [ ] Change procedure been tested successfully <!-- to validate once the chain TESTNET has been started in DEV or QA -->

<!-- Include step by step description -->

 - [ ] identify external public nodes for node drift monitoring
 ```
list + add to prometheus monitoring stack as 'external'
* ...
 ```
 - [ ] (Ops team) DNS : declare new entries to respective servers (note : can be done ASAP for all environments, no impact on actual servers)
 - [ ] (Ops team) create templates for deploying new versions & adapt ansible inventory
 - [ ] (Ops team) run ansible playbook to deploy the new templates + start the new chain:
   - [ ] DEV environment  created
   - [ ] QA environment   created
   - [ ] PROD environment created
 - [ ] (Ops team) Deploy endpoints umami-stack-orchestration 
    - [ ] DEV environment  started: http://TESTNET-dev.umamiwallet.com/version
    - [ ] QA environment   started: http://TESTNET-qa.umamiwallet.com/version
    - [ ] PROD environment started: http://TESTNET.umamiwallet.com/version
    - [ ] add monitoring (external nodes and umami-stack)

 - [ ] (Dev team) ~"App::Frontend" add option in Umami client (TODO : link this issue to the MR)
(To be released only once endpoints started in Production, can be tested in QA or DEV as soon as the endpoints are available)
```
/create_merge_request Add test-net for TESTNET in umami client
/label ~"state::Backlog" ~dev 
/assign @sagotch @leoparis89
```

## Rollback plan
<!-- Describe how to rollback the change in case the expected change is not working -->

* stop endpoints (Operations team)
* remove monitoring (Operations team)
* remove option in umami client (Development team)

<!-- METADATA for project management, please leave the following lines and edit as needed -->
# Metadata
<!-- PRIORITY: Uncomment /label quick actions as appropriate. The priority and severity assigned may be different to this !-->
<!--High : (This will bring a huge increase in performance/productivity/usability, or is a legislative requirement)-->
<!-- /label ~"Priority::1-High" -->
<!--Medium : (This will bring a good increase in performance/productivity/usability)-->
<!--/label ~"Priority::2-Medium" -->
<!--Low : (anything else e.g., trivial, minor improvements) -->
<!--  /label ~"Priority::3-Low" -->

## Approvals checklist (all required) 
- [ ] Approval from Development (once DEV or QA endpoints validated)
- [ ] Approval from Operations (once all minimal versions validated)
- [ ] Approval from Business
<!-- tick the corresponding checkbox [x], you may also add your @user handle at the end of the line -->

<!-- Trigger gitlab todo tasks --> 

/todo   @sagotch @leoparis89           Please *approve* this _regular change_ (new test-net) on development aspects + provide `mezos` version compatible with TESTNET + validate QA or DEV endpoints.

/todo  @comeh (cc: @philippewang.info) Please *approve* this _regular change_ (new test-net) on operations aspects + start backends and monitoring for TESTNET.

/todo @SamREye                         Please *approve* this _regular change_ (new test-net) on business    aspects + provide Umami-client version/s supporting TESTNET.

/todo @philippewang.info Please provide `tezos-indexer` version compatible with TESTNET.

/assign @SamREye @comeh @sagotch @leoparis89

<!-- Quick actions for last approver : -->
<!-- /unlabel ~"CAB::to-approve" -->
<!-- /label ~"CAB::to-perform"   -->

/label ~"Change::Regular" <!-- Regular change, to be approved by the CAB before applying. -->
/label ~Change ~"CAB::to-approve" <!-- labels for gitlab CAB Change issues management -->
<!-- METADATA - end -->
