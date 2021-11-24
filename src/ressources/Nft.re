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

type ressource =
  | IPFS(string)
  | HTTP(string);

type images = {
  thumbnail: option(ressource),
  artifact: option(ressource),
  display: option(ressource),
};

type t = {
  images,
  name: string,
  id: int,
  hidden: bool,
  contract: PublicKeyHash.t,
  description: option(string),
};

type contract = {
  tokens: list(t),
  name: string,
  address: PublicKeyHash.t,
};

type uniqueKey = (PublicKeyHash.t, int);

module KeyCompare =
  Belt.Id.MakeComparable({
    type t = uniqueKey;
    let cmp = Pervasives.compare;
  });

let toRessource = uri =>
  uri->Js.String2.startsWith("ipfs://")
    ? uri->Js.String2.substringToEnd(~from=7)->IPFS : HTTP(uri);

let fromToken = (t: TokenRepr.t): option(t) => {
  t->TokenRepr.isNFT
    ? Some({
        images: {
          thumbnail: t.asset.thumbnailUri->Option.map(toRessource),
          artifact: t.asset.artifactUri->Option.map(toRessource),
          display: t.asset.displayUri->Option.map(toRessource),
        },
        name: t.alias,
        id: t->TokenRepr.id,
        contract: t.address,
        description: t.asset.description,
        hidden: false,
      })
    : None;
};

let toTokenRepr = (nft: t): TokenRepr.t => {
  alias: nft.name,
  kind: FA2(nft.id),
  address: nft.contract,
  symbol: "",
  chain: "",
  decimals: 0,
  asset: TokenRepr.defaultAsset,
};

let uniqueKey = (contract: PublicKeyHash.t, id) =>
  (contract :> string) ++ "-" ++ Int.toString(id);

let flatten = contracts =>
  contracts->List.reduce(Map.make(~id=(module KeyCompare)), (acc, c) =>
    c.tokens
    ->List.reduce(acc, (acc, tok) =>
        acc->Map.set((tok.contract, tok.id), tok)
      )
  );

let fromCache = cache =>
  cache->PublicKeyHash.Map.reduce(
    [],
    (acc, address: PublicKeyHash.t, contract: TokenRegistry.Cache.contract) => {
      let tokens =
        contract.tokens
        ->Map.Int.reduce([], (acc, _, t) =>
            switch (t) {
            | TokenRegistry.Cache.Partial(_) => acc
            | Full(t) =>
              let nft = t->fromToken;
              List.addOpt(acc, nft);
            }
          );
      tokens == []
        ? acc
        : [
          {
            tokens,
            address,
            name: contract.name->Option.getWithDefault((address :> string)),
          },
          ...acc,
        ];
    },
  );
