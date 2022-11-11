<!-- "Regular Change" template for test-net end of life : 
* step 1/ copy & paste in new issue https://gitlab.com/nomadic-labs/umami-wallet/umami/-/issues/new?issue[title]=Test-net%20End%20Of%20Life:%20TESTNET
* step 2/ (Case-sensitive) search & replace 'TESTNET' (Case sensitive) by the test-net name in this document
* -->

<!-- /title Test-net End of Life: TESTNET  -->
/label  ~"Umami-stack" ~"Umami-stack::monitoring" ~"App::Frontend"  ~"tezos-net::testnet"
/label ~"Priority::2-Medium"
/assign @SamREye @comeh @sagotch @leoparis89

[[_TOC_]]
## (Regular change) End of test-net TESTNET
<!-- Regular change, to be approved by the CAB before applying. -->

<!-- /confidential -->
<!-- If confidential, explain why -->


### Summary
<!-- Outline the issue being faced, and why this required a change !-->
Stop the support for TESTNET, first:
* first remove it from Umami client options 
* then stop the production endpoint(s)
* remove the monitoring

### Area of the system
<!-- This might only be one part, but may involve multiple sections !-->

* umami-backend  ~"Umami-stack" endpoints running on servers.
* umami-frontend : client options for users

 ~"Umami-stack" ~"Umami-stack::monitoring" ~"App::Frontend"

### How does this currently work?
<!-- The current process, and any associated business rules !-->

* ~"Umami-stack" : dev, qa, prod have endpoints for TESTNET
  * ~"Umami-stack::monitoring" : has endpoints towards those services

* ~"App::Frontend" umami has a default setting for TESTNET


### What is the desired way of working?
<!-- After the change, what should the process be, and what should the business rules be !-->
* TESTNET is not present by default in Umami client
* Monitoring for TESTNET is removed
* Endpoints for TESTNET are stopped 

<!-- Success criteria of change application (when relevant, include how to test) -->

### Change Procedure
- [ ] Change procedure been tested successfully

<!-- Include step by step description -->
* Step by step :

  - [ ] Pre-requesite ~"App::Frontend": MR & Release to remove TESTNET endpoint in Umami client  (@sagotch @leoparis89 / Dev Team, please link to this issue with `/relate umami#999` )
    - Note :warning: do not stop the production endpoint before MR is merged and released
  - [ ] ~"Umami-stack::monitoring": comment out in prometheus.yml all entrypoints specific to this tezos-network endpoints (easier for search & replace for next test-net to implement, no impact in leaving it commented)
  - [ ] Take backups of the node and indexed DB (to restore if rollback needed in the next few days) :
```
ssh qa-api
cd /opt/umami-backend/amino/TESTNET
make pg_dump
make node_backup
```
  - [ ] stop TESTNET (dev, qa, prod) but do not purge the data already
```
cd /opt/umami-backend/amino/TESTNET
make stop
```
  - [ ] (after stopping everything) set due date to now + 1 week (`/due 1w`)

* After 1 week : remove the data
```
# remove docker volumes that might still be present (make sure that all other env are up and running since this will purge unused volumes
cd /opt/umami-backend/amino/
make prune
#remove any backup left :
rm /opt/umami-backend/amino/TESTNET/import-data/*
```

## Rollback plan
<!-- Describe how to rollback the change in case the expected change is not working -->

During the 1 week after stopping and before removing the backups, to restart without having to download the full TESTNET chain :
```
cd /opt/umami-backend/amino/TESTNET
make pg_dump_restore
make node_backup_restore
```

After the backups have been deleted : 
 * start everything from scratch (node p2p sync, and indexing the node data)

<!-- METADATA for project management, please leave the following lines and edit as needed -->
# Metadata
/label ~"Change::Regular" <!-- Regular change, to be approved by the CAB before applying. -->
/label ~Change ~"CAB::to-approve" <!-- labels for gitlab CAB Change issues management -->

<!-- PRIORITY: Uncomment /label quick actions as appropriate. Priority and Severity may hold different values! -->
<!--High : (This will bring a huge increase in performance/productivity/usability, or is a legislative requirement)-->
<!-- /label ~"Priority::1-High" -->
<!--Medium : (This will bring a good increase in performance/productivity/usability)-->
<!-- /label ~"Priority::2-Medium" -->
<!--Low : (anything else e.g., trivial, minor improvements) -->
<!--  /label ~"Priority::3-Low" -->

## Approvals checklist (all required) 
<!-- tick the corresponding checkbox [x], you may also add your @user handle at the end of the line -->
- [ ] Approval from Business (are we clear to stop TESTNET support in Umami)
- [ ] Approval from Operations 
- [ ] Approval from Development (once Umami clients have been released without the option)
   - [ ] removed from Umami Desktop latest release
   - [ ] removed from Umami Mobile latest release

/assign @sagotch @leoparis89 @comeh @SamREye 

<!-- Trigger gitlab todo tasks --> 

@sagotch @leoparis89            Please *approve* this _regular change_ on development aspects

@comeh (cc: @philippewang.info) Please *approve* this _regular change_ on operations  aspects

@SamREye                        Please *approve* this _regular change_ on business    aspects : are we clear to stop TESTNET support in Umami ?

<!-- Quick actions for last approver : -->
<!-- /unlabel ~"CAB::to-approve" -->
<!-- /label ~"CAB::to-perform"   -->

<!-- METADATA - end -->
