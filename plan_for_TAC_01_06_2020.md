# Project Plan: Reference Wallet within Nomadic Labs

Dear all, here is the curent status of the new wallet effort that has been discussed for the last few weeks. The plan is to draw the big picture, and to let Sam handle the initial recruitment and process definiton.

## What do we mean by reference wallet

### The wallet we are going to build will:

 - Be implemented in ReasonML (probably using ReasonReact).
 - Implement a maximum of features of Tezos (at least as much as the CLI).
 - Serve as a reference to other wallet developers.
 - Conform to the wallet working group conventions.
 - Have a desktop UI first, can be a webview.
 - Provide good UX: people should not struggle to use it.
 - Can be an expert-user oriented UX, but that does not mean bad UX.
 - Have a responsive UI that does not hide the asynchronous nature of the chain.
 - Look not too ugly, but that's not a priority.
 - Be split into the SDK part and the UI part, so that others can build alternative UIs.

### Why should Nomadic build a reference wallet?

 - It makes sense to have a demonstrator wallet built close to a core development team.
 - We believe we have a good comparative success rate for building a reference wallet.
 - We'll be able to showcase new features as soon as they reach testnet/injection.
 - We have some experience (tezos-client, Cortez)

## HR aspects

### Roles needed:

 - Experienced ReasonML lead developers (2 recruits).
 - Project management (Sam).
 - Community coordinator (other wallets, other actors users).
 - UI/UX advisor.
 - Junior ReasonML developers.
Some roles could be attributed to the same person or shared.

### What happens of the current wallet team?

 - Nomadic will stop developing the current Cortez.
 - Nicolas and Steve will be asked for a migration update for current users.
 - They will be offered to join the new team as junior devs, in order to not waste their experience.

## Risks

  - The market for ReasonML developers is tense, and we may not be able to find the profiles we need. Mitigation: None, we'll have to try and see.
  - We've never hired UX specialists. Mitigation: we'll have to get help for this.
  - We'll give the option to our current mobile developers to switch careers to ReasonML developers, which may not work. Mitigation, we'll have to monitor how this goes, and set a date, with them, at which we'll decide if we stop or continue.
  - Underestimation of the difficulty: if this was an easy task, we would already have reference wallets. Mitigation: we'll have to take project planning and continuous risk assessment seriously to avoid delays of failures.
  - Pressure on the current dev team. Mitigation: the proposed team structure should help, we want a dedicated management role, and experienced developers in a technology that's not too foreign to the core dev team.

## Roadmap

  - Q3 2020:
    - Recruit the new team members
    - Clean stop of Cortez / convert the Cortez team to Reason ML
  - Q4 2020: basic wallet + stable development workflow
  - Q2 2021: reference wallet (beta release)
  
