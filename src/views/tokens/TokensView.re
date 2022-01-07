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

open ReactNative;

module AddTokenButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginLeft=(-6.)->dp,
            ~marginBottom=10.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~tokens, ~chain=?) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();

    let tooltip =
      chain == None
        ? Some(("add_token_button", I18n.Tooltip.chain_not_connected)) : None;

    <>
      <View style=styles##button>
        <ButtonAction
          disabled={chain == None}
          ?tooltip
          onPress
          text=I18n.Btn.add_token
          icon=Icons.Add.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView
          chain={chain->Option.getWithDefault("")}
          tokens
          closeAction
        />
      </ModalAction>
    </>;
  };
};

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let apiVersion: option(Network.apiVersion) = StoreContext.useApiVersion();

  let accounts = accounts->PublicKeyHash.Map.keysToList;
  let request = fromCache =>
    TokensApiRequest.{
      request: Fungible.{accounts, numberByAccount: BCD.requestPageSize},
      fromCache,
    };

  // will be used to indicate a percentage of tokens loaded
  let onTokens = (~total as _, ~lastToken as _) => ();
  let onStop = () => false;

  let (tokensRequest, _) =
    StoreContext.Tokens.useFetchTokens(
      onTokens,
      onStop,
      accounts,
      request(true),
    );

  let tokens =
    switch (tokensRequest) {
    | NotAsked
    | Loading(None) => None

    | Loading(Some(`Cached(tokens) | `Fetched(tokens, _)))
    | Done(Ok(`Cached(tokens) | `Fetched(tokens, _)), _) =>
      Some(Ok(tokens))

    | Done(Error(error), _) => Some(Error(error))
    };

  let partitionedTokens =
    React.useMemo1(
      () => {
        tokens->Option.map(tokens =>
          tokens->Result.map(tokens =>
            tokens->TokensLibrary.Generic.keepPartition((_, _, (t, reg)) =>
              t->TokensLibrary.Token.isNFT ? None : Some(reg)
            )
          )
        )
      },
      [|tokens|],
    );

  let currentChain = apiVersion->Option.map(v => v.chain);

  <Page>
    <Typography.Headline style=Styles.title>
      I18n.Title.tokens->React.string
    </Typography.Headline>
    <AddTokenButton
      chain=?currentChain
      tokens={
        tokens->Option.mapDefault(TokensLibrary.Generic.empty, t =>
          t->Result.getWithDefault(TokensLibrary.Generic.empty)
        )
      }
    />
    {switch (partitionedTokens) {
     | None => <LoadingView />
     | Some(Error(error)) => <ErrorView error />
     | Some(Ok((registered, unregistered))) =>
       <>
         <TokenRows
           title=I18n.Title.added_to_wallet
           tokens=registered
           currentChain
         />
         <TokenRows title=I18n.Title.held tokens=unregistered currentChain />
       </>
     }}
  </Page>;
};
