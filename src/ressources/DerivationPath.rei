/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

type error =
  | ParsingFailed(string)
  | MoreThan1Wildcard
  | MissingWildcardOr0
  | NotTezosBip44;

exception IllFormedPath;

let handleError: error => string;

// A derivation path
// see https://github.com/bitcoin/bips/blob/master/bip-0044.mediawiki
type t;

type derivationPath = t;

let toString: t => string;

module Pattern: {
  /* A derivation path pattern
     Building functions enforces the following invariants:
     - Contains at most one wildcard: ?'
     - If no wildcard, it contains at least a: 0' */
  type t;

  // A derivation path pattern following BIP44 standard
  type tezosBip44;

  // The default value for derivation path
  let default: tezosBip44;

  // The default value for derivation path as a string
  let defaultString: string;

  let convertToTezosBip44: t => Result.t(tezosBip44, error);

  let fromTezosBip44: tezosBip44 => t;

  let isDefault: t => bool;

  let toString: t => string;

  /* Implements a derivation path pattern
     Search for the unique wildcard an replaces it with the
     given index. If there is no wildcard, replaces the last 0.
     By construction of a derivation path pattern, [implement] cannot fail. */
  let implement: (t, int) => derivationPath;

  /* Builds a derivation path pattern from a string enforcing previously defined
     invariants */
  let fromString: string => Result.t(t, error);
};