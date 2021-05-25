# Umami Wallet

## Purpose

This document lists approaches taken by the Umami team to ensure fitness of the Umami Wallet product.

## Goals

 * To ensure that the product is release as fit-for-purpose:
   * secure
   * reliable
   * usable
 * To ensure that changes are rolled-out safely

 Out of scope: resiliency and Disaster Recovery--treated separately in another policy

## Overview

To best approach to meet stated goals is a combination of approaches:

| Testing Type | Description | Problem Addresssed |
|--|--|--|
| Unit Testing | testing of sub-components in isolation | useful for identifying low-level breakages in CI |
| Functional Testing | testing of an aggregate of components as a system (or sub-system) | Identifies issues between components or systems as a whole |
| UI/UX Testing | testing performed by humans following a checklist on the fully built product | Verifies and validates end product |
| Benchmarking | testing tailored to measure resiliency and performance | Identifies performance issues and tracks trends in performance |

## Unit Testing

Unit tests are tests written in ReasonML that test simple parts of the source code. They assist in catching fundamental problems before they manifest in the larger product/system.

 * Defined in: (/__tests__/)[/__tests__/]
 * Kick off by: CI, every git push
 * Results in: CI logs

## Functional Testing

Functional tests are tests around an interface or sub-system so that largers parts of code can be tested in unison. They assist in identifying fundamental problems between components.

 * Defined in: ()[]
 * Kick off by: 
 * Results in: 

## UI/UX Testing

UI/UX Testing verifies that the final product is fit-for-purpose.

 * Defined in: (/docs/testing/)[/docs/testing/]
 * Kick off by: when a new infrastructure rollout or a new version of Umami is announced internally
 * Results in: (/docs/testing/runs/)[/docs/testing/runs/]

## Benchmarking

Benchmarking is a method for measuring and tracking performance of the product. Identifies the need for and impact of optimizations.

 * Defined in: ()[]
 * Kick off by: when a new infrastructure rollout or a new version of Umami is announced internally
 * Results in: (/docs/testing/benchmarks/)[/docs/testing/benchmarks/]



