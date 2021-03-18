# Umami Wallet - Test Checklist

**Date**: [DATE]

## Test Parameters

### Components

| Component | Tag or Branch |
|--|--|
| umami | [TAG or BRANCH] |
| tezos-indexer | [TAG or BRANCH] |
| mezos | [TAG or BRANCH] |
| tezos | [TAG or BRANCH] |

### Settings

| Setting | Value |
|--|--|
| Node | [VALUE] |
| Mezos | [VALUE] |

## Checklist

> Legend: ticking the box means: (1) it completed successfully; or (2) completed partially without defect; or (3) failed but was recoverable. If an issue comes up that is not severe and is recoverable: file an issue, but still tick the box as successful.

### Accounts and Operations

 - [ ] Send normal transaction
 - [ ] Observe block count and confirmation
 - [ ] Observe explorer, use explorer link
 - [ ] Import secret
 - [ ] Create derived Account
 - [ ] Rename derived Account
 - [ ] Send transaction to or from a derived Account
 - [ ] Send a Batch transaction
 - [ ] Edit a transaction within a Batch
 - [ ] Delete a transaction within a Batch
 - [ ] Export a secret
 - [ ] Rename a secret
 - [ ] Delete a secret
 - [ ] Select Advanced Options

### Address Book

 - [ ] Create alias
 - [ ] Observe c&p and QR code
 - [ ] Rename alias
 - [ ] Delete alias
 - [ ] Observe alias in Send form

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

 - [ ] Change verification blocks setting and Observe
 - [ ] Change dark/light mode
 - [ ] Reset settings

## Issues

 * [ISSUE - Example: #117]
