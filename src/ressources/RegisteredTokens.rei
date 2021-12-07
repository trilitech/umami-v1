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

/** Representation of registered tokens for a user */
type nftInfo = {
  holder: PublicKeyHash.t,
  hidden: bool,
  balance: ReBigNumber.t,
};

type kind =
  | FT
  | NFT(nftInfo);

type contract = {
  contract: TokenContract.t,
  chain: string,
  tokens: Map.Int.t(kind) // effectively registered tokens by the user
};

type t = PublicKeyHash.Map.map(contract);

let isRegistered: (t, PublicKeyHash.t, int) => bool;
let registerToken: (t, Token.t, kind) => t;
let removeToken: (t, PublicKeyHash.t, int) => t;
let isHidden: (t, PublicKeyHash.t, int) => bool;
let keepTokens: (t, (PublicKeyHash.t, int, kind) => bool) => t;
let updateNFTsVisibility: (t, NftSelection.t, bool) => t;
let updateNFT: (t, PublicKeyHash.t, int, nftInfo) => t;
