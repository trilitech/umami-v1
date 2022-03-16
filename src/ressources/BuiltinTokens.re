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

module Templates = PublicKeyHash.Map;

let hicetnunc = {
  let address =
    "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"->PublicKeyHash.build->Result.getExn;
  (
    address,
    (tokenId, asset) =>
      TokenRepr.{
        kind: TokenRepr.FA2(tokenId),
        address,
        alias: Format.sprintf("HEN#%d", tokenId),
        symbol: Format.sprintf("HEN#%d", tokenId),
        chain: `Mainnet->Network.getChainId,
        decimals: 0,
        asset:
          asset->Option.getWithDefault({
            ...Metadata.defaultAsset,
            isBooleanAmount: true,
          }),
      },
  );
};

let addTemplate = (templates, (address, template)) =>
  templates->Templates.set(address, template);

let templates = Templates.empty->addTemplate(hicetnunc);

let findTemplate = (address, tokenId, asset) => {
  templates
  ->Templates.get(address)
  ->Option.map(template => template(tokenId, asset));
};
