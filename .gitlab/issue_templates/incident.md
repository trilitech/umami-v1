[[_TOC_]]
## Incident Report

<!-- ### Checklist -->
<!-- Before filing an incident report, if there are any doubt, follow the checklist to ensure you are in fact dealing with an incident: !-->
<!--
 - [] 1. the situation impacts the live production environment
 - [] 2. the situation impacts the users access to mainnet chain services or assets
 - [] 3. the situation is severe or it carries a high risk; in that **any** of the following is true:
   -  [] it will/may lead to users being unable to view their assets 
   -  [] it will/may lead to users being unable to access their assets
   -  [] it will/may lead to downtime of the system
   -  [] it will/may lead to downtime to a feature of the system (which is not purely informational)
 - [] 4. the adverse impact is urgent; in that **all** of the following is true:
   - [] is ongoing or is imminent
   - [] no straightforward workaround is possible
   - [] solution cannot wait for the next planned release
-->
<!-- If you can tick boxes 1, 2, 3, and 4--you have uncovered an Incident; otherwise, it may not be, please consult management or you may file it anyway if unsure. !-->

### What is the nature of the incident?
<!-- Which parts of the system where affected ? -->


### How the incident was discovered?
<!-- Mention which alert was triggered where, but report wrong or missing alerts when it's the case -->
<!-- slack ? manual check ? user notification ? other -->


### What is the potential impact of the incident?
<!-- What is the user impact of the incident-->


### What is the evidence (i.e. screenshots, logs, etc)?
<!-- Surround logs exctacts with ``` multiline logs```-->



<!-- METADATA for project management, please leave the following lines and edit as needed -->
# Metadata
<!-- Severity : pick one the gitlab panel, right side of the window when viewing the incident (after creation) -->

/label ~incident  
<!-- Labels and default review status for gitlab Change management process, comment if no change was performed-->
/label ~Change ~"CAB::to-review" ~"Change::Emergency" 

## Reviews checklist (all required)
- [ ] Incident Review from Development 
- [ ] Incident Review from Operations 
- [ ] Incident Review from Business 
<!-- tick the corresponding checkbox [x], you may also add your @user handle at the end of the line -->

<!-- Trigger gitlab todo tasks --> 

@picdc (cc: @remyzorg )    Please *review* this _emergency change_ on development aspects

@comeh (cc: @philippewang.info) Please *review* this _emergency change_ on operations aspects

@SamREye                   Please *review* this _emergency change_ on business aspects

<!-- /assign @picdc @comeh @SamREye -->

<!-- Quick actions for last reviewer : -->
<!-- /unlabel ~"CAB::to-review" -->

<!-- METADATA - end -->
