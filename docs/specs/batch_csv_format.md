# Umami Wallet - Batch File Format Specifications

[[_TOC_]]

## Example

```
# This is an example CSV file for a Tezos batch of transactions
tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456 # this is a simple tez tx -- all comments are optional
tz1cbGwhSRwNt9XVdSnrqb4kzRyRJNAJrQni,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2 # this is a simple token tx
tz1cbGwhSRwNt9XVdSnrqb4kzRyRJNAJrQni,2000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton # this is a simple token tx -- defaults to tokenid 0 or ignored if a single asset contract
```

## Abstract

Batch File Format Specifications, as defined herein, allow convenience to scale large batches with the additional benefit of reusability and interoperability across wallets that implemente this specification. From a simple, human-readable CSV file, a batch can be loaded into a wallet ready for signature.

## Introduction

Batches offer convenience over simple transactions in that they are validated in one block, plus they are economical both in fees and resources.

For large batches however, it may be tedious to enter the transactions one by one on a wallet user interface. As such, loading of transactions as a CSV definition allows for simple and quick entry of large bulk transactions.

## Definition of the Batch Format

### Overview

The file format defined herein describes a batch of Tezos operations in the [CSV format](https://datatracker.ietf.org/doc/html/rfc4180).

A batch is a collection of operations, which in this definition are necessarily transactions (although they may imply operations, such as reveals).

Any non-empty and non-commented line in the batch file, that matches a transaction specification as per the section below, describes a transaction.

All transactions described by a CSV line in the file compose the batch.

#### Formal Specification

```
file = [comment CRLF] transaction *(CRLF transaction) [comment] [CRLF]
transaction = teztx | tokentx
teztx = destination COMMA amount CRLF
tokentx = destination COMMA amount COMMA tokenaddr *(COMMA tokenid)
comment = HASHTAG text
destination = tz[123][A-Za-z0-9]+{33}
amount = [0-9]+(.[0-9]*)
tokenaddr = KT1[A-Za-z0-9]+{33}
tokenid = [0-9]+
COMMA = %x2C
HASHTAG = %x23
```

### Transaction

In a wallet setting, the sending account is to be specified outside of the batch definition.

The justification for this is that the batch is agnostic of where the transaction originates; in this way, the batch may be shared and reused.

#### Simple Tez Transaction

As per the [specification](#Formal Specification)'s `teztx` definition, the base-token tez transaction requires:
1. a tz(1|2|3) address as the intended destination; and
1. the amount of tez to send

#### Token Transaction

As per the [specification](#Formal Specification)'s `tokentx` definition, a token transaction requires:
1. a tz(1|2|3) address as the intended destination;
1. the amount of token to send;
1. the address to the contract that manages the token; and
1. (optional) the token_id of the token within the contract

## Exception Handling

If any line does not match a transaction specification, the line, the transaction, the file and the batch are considered invalid.

