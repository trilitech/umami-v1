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

include ApiRequest;

let useLoadMetadata = (cache, pkh, kind) => {
  let keepTaquitoErrors =
    fun
    | TokensAPI.NotFAContract(_)
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true;

  let id = kind->TokenRepr.kindId;

  let buildContract = (config: ConfigContext.env) => {
    let toolkit = MetadataAPI.toolkit(config);
    MetadataAPI.Tzip12.makeContract(toolkit, pkh)
    ->Promise.flatMapOk(contract => MetadataAPI.Tzip12.read(contract, id));
  };

  let get = (~config, ()) =>
    switch (cache->TokensLibrary.Generic.getToken(pkh, id)) {
    | Some((t, _)) => Promise.ok(t)
    | None =>
      buildContract(config)
      ->Promise.mapOk(metadata =>
          TokensAPI.metadataToToken(
            config.network.Network.chain->Network.getChainId,
            TokenContract.{address: pkh, kind: kind->fromTokenKind},
            metadata,
          )
          ->TokensLibrary.Token.Full
        )
    };

  let requestState = React.useState(() => ApiRequest.NotAsked);

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Tokens,
    ~requestState,
    ~keepError=keepTaquitoErrors,
    (),
  );
};
