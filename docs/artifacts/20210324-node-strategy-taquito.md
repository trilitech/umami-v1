# Umami Strategy

## Background

 * Umami is behind on its release
   * admittedly, this could have been avoided had we integrated Taquito from the start, as Arthur originally advised

## Cause of Delay

 * Umami was sidetracked/distracted by a desired tezos-client compiled in JS to be used as an SDK
 * The SDK timelines slipped exessively--due to its degree of difficulty

## Goals

 * To release Umami:
   * with either Taquito; or
   * with the SDK that meets a reliability standard (via testing, verification, validation)

## Estimates

| Approach | Optimistic | Pessimistic | Quality/Reliability |
|--|--|--|--|
| SDK | 2 weeks* | 5 weeks* | No guarantees on reliability--could likely impose additional testing and remediation |
| Taquito | 3 weeks | 4 weeks | Known quantity |

NOTE (*): SDK's estimates are risk adjusted due to past history of delays.

## Findings

 * **Taquito represents both a fast and safe option: it is clearly the way forward**
 * Parts of the tezos-client may be used in parts for a reuse of the local key management and aliases; this will accelerate the migration to Taquito

## Direction

Unless otherwise directed, Team Umami intends to proceed as follows:
 * To primarily focus on Taquito integration for Node RPC calls [Pierrick, Steve and Remy]
   * To keep tezos-client's approach to key management (which is not included in Taquito)
 * As a second priority, to keep advancing on SDK [Alexandre and Remy (partial)]
 * Without adversely impacting the main goal, to keep Umami compatibility with both Taquito and the SDK

## Commitment

On or before **April 16**, to have a working release of Umami.
