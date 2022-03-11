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
  chain: Network.chainId,
  tokens: Map.Int.t(kind) // effectively registered tokens by the user
};

type t = PublicKeyHash.Map.map(contract);

let isRegistered = (registered, pkh, tokenId) => {
  registered
  ->PublicKeyHash.Map.get(pkh)
  ->Option.map(t => t.tokens->Map.Int.has(tokenId))
  != None;
};

let registerToken = (registered, token: Token.t, kind) => {
  registered->PublicKeyHash.Map.update(
    token.address,
    fun
    | None =>
      Some({
        contract:
          TokenContract.{
            address: token.address,
            kind: TokenContract.fromTokenKind(token.kind),
          },
        chain: token.chain,
        tokens: Map.Int.empty->Map.Int.set(TokenRepr.id(token), kind),
      })
    | Some(t) =>
      Some({
        ...t,
        tokens: t.tokens->Map.Int.set(TokenRepr.id(token), kind),
      }),
  );
};

let removeToken = (registered, pkh, tokenId) =>
  registered->PublicKeyHash.Map.update(
    pkh,
    fun
    | None => None
    | Some(t) => {
        let tokens = t.tokens->Map.Int.remove(tokenId);
        tokens->Map.Int.isEmpty ? None : Some({...t, tokens});
      },
  );

let isHidden = (registered, pkh, tokenId) =>
  registered
  ->PublicKeyHash.Map.get(pkh)
  ->Option.flatMap(t => t.tokens->Map.Int.get(tokenId))
  ->Option.mapWithDefault(
      false,
      fun
      | FT => false
      | NFT({hidden}) => hidden,
    );

let updateNFTsVisibility = (registered, tokens, hidden) => {
  let mergeTokens = (_, t, updatedId) =>
    switch (t, updatedId) {
    | (None, _) => None
    | (Some(_), None)
    | (Some(FT), _) => t
    | (Some(NFT(infos)), Some ()) => Some(NFT({...infos, hidden}))
    };

  let mergeContracts = (_, c, ids) =>
    switch (c, ids) {
    | (None, _) => None
    | (Some(_), None) => c
    | (Some(regContract), Some(updatedTokens)) =>
      {
        ...regContract,
        tokens: Map.Int.merge(regContract.tokens, updatedTokens, mergeTokens),
      }
      ->Some
    };

  PublicKeyHash.Map.merge(registered, tokens, mergeContracts);
};

let updateNFT = (registered, pkh, tokenId, nftInfo) =>
  registered->PublicKeyHash.Map.update(
    pkh,
    fun
    | None => None
    | Some(t) => {
        let tokens =
          t.tokens
          ->Map.Int.update(
              tokenId,
              fun
              | Some(NFT(_)) => Some(NFT(nftInfo))
              | k => k,
            );
        Some({...t, tokens});
      },
  );

let keepTokens = (registered, f) =>
  registered->PublicKeyHash.Map.reduce(
    PublicKeyHash.Map.empty,
    (registered, pkh, c) => {
      let tokens = c.tokens->Map.Int.keep((id, token) => f(pkh, id, token));
      tokens->Map.Int.isEmpty
        ? registered : registered->PublicKeyHash.Map.set(pkh, {...c, tokens});
    },
  );
