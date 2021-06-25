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
  let make = (~chain=?) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();

    let tooltip =
      chain == None
        ? Some(("add_token_button", I18n.tooltip#chain_not_connected)) : None;

    <>
      <View style=styles##button>
        <ButtonAction
          disabled={chain == None}
          ?tooltip
          onPress
          text=I18n.btn#add_token
          icon=Icons.Add.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView chain={chain->Option.getWithDefault("")} closeAction />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(StyleSheet.create({"list": style(~paddingTop=4.->dp, ())}));

[@react.component]
let make = () => {
  let tokensRequest = StoreContext.Tokens.useRequest();
  let apiVersion: option(Network.apiVersion) = StoreContext.useApiVersion();

  <Page>
    <AddTokenButton chain=?{apiVersion->Option.map(v => v.chain)} />
    <Table.Head>
      <TokenRowItem.CellName>
        <Typography.Overline3>
          I18n.t#token_column_name->React.string
        </Typography.Overline3>
      </TokenRowItem.CellName>
      <TokenRowItem.CellSymbol>
        <Typography.Overline3>
          I18n.t#token_column_symbol->React.string
        </Typography.Overline3>
      </TokenRowItem.CellSymbol>
      <TokenRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#token_column_address->React.string
        </Typography.Overline3>
      </TokenRowItem.CellAddress>
    </Table.Head>
    <View style=styles##list>
      {switch (tokensRequest) {
       | NotAsked
       | Loading(None) => <LoadingView />
       | Loading(Some(tokens))
       | Done(Ok(tokens), _) when tokens->Map.String.size == 0 =>
         <Table.Empty> I18n.t#empty_token->React.string </Table.Empty>
       | Loading(Some(tokens))
       | Done(Ok(tokens), _) =>
         tokens
         ->Map.String.valuesToArray
         ->Array.map(token =>
             <TokenRowItem key=(token.address :> string) token />
           )
         ->React.array
       | Done(Error(error), _) => <ErrorView error />
       }}
    </View>
  </Page>;
};
