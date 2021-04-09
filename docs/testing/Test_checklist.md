# Umami Wallet - Test Checklist

**Date**: [DATE]

## Test Parameters

### OS

| Setting | Value |
|--|--|
| Operating System | [Mac or Linux] |
| Flavor (if applicable) | [N/A for Mac] |

### Components

| Component | Tag or Branch |
|--|--|
| umami | [TAG] |
| tezos-indexer | [TAG] |
| mezos | [TAG] |
| tezos | [TAG] |

### Settings

| Setting | Value |
|--|--|
| Node | [VALUE] |
| Mezos | [VALUE] |

## Checklist

> Legend: ticking the box means: (1) it completed successfully; or (2) completed partially without defect; or (3) failed but was recoverable. If an issue comes up that is not severe and is recoverable: file an issue, but still tick the box as successful.

### Onboarding
- [ ] Create new account
- [ ] Import account

### Accounts and Operations
- [ ] Send normal transaction
- [ ] Observe block count and confirmation
- [ ] Observe explorer, use explorer link
- [ ] Send transaction to or from a derived Account
- [ ] Send a Batch transaction
- [ ] Edit a transaction within a Batch
- [ ] Delete a transaction within a Batch
- [ ] Select Advanced Options
- [ ] Import account
- [ ] Export a secret
- [ ] Rename a secret
- [ ] Delete a secret
- [ ] Create derived Account
- [ ] Rename derived Account
- [ ] Select Advanced Options
- [ ] Add contact from Operations

### Address Book
- [ ] Create alias
- [ ] Observe c&p 
- [ ] Observe QR code
- [ ] Rename alias
- [ ] Observe alias in Send form
- [ ] Delete alias


### Delegations
- [ ] Delegate an Account
- [ ] Observe the delegation
- [ ] Change baker
- [ ] Delete delegation

### Tokens
- [ ] Register token FA1.2 contract
- [ ] Delete token contract
- [ ] Send token
- [ ] Send token batch

### Settings
- [ ] Change verification blocks setting 
- [ ] Observe new blocks setting in Operations
- [ ] Change dark/light mode
- [ ] Reset settings
- [ ] Offboard wallet

### Issues to Verify

 * [ ] [ISSUE_NUM - Example: #117]

## Issues Found

 * [ISSUE_NUM - Example: #117]
