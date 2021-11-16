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

let styles =
  Style.(
    StyleSheet.create({
      "selector":
        style(
          ~alignSelf=`flexStart,
          ~maxWidth=320.->dp,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          (),
        ),
      "selectorContent":
        style(
          ~height=40.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "selectorDropdown": style(~minWidth=275.->dp, ~left=auto, ()),
      "spacer": style(~height=6.->dp, ()),
    })
  );

type token =
  | Tez
  | Token(Token.t);

let symbol =
  fun
  | Tez => I18n.t#tez
  | Token(t) => t.symbol;

let name =
  fun
  | Tez => I18n.t#tezos
  | Token(t) => t.alias;

module TokenItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "inner":
          style(
            ~height=22.->dp,
            ~marginLeft=14.->dp,
            ~marginRight=10.->dp,
            ~flex=1.,
            ~flexDirection=`row,
            ~justifyContent=`spaceBetween,
            (),
          ),
        "titleContainer": style(~flexDirection=`row, ~alignItems=`center, ()),
        "icon": style(~marginRight=10.->dp, ()),
      })
    );

  [@react.component]
  let make = (~token: token) => {
    let theme = ThemeContext.useTheme();

    <View style=styles##inner>
      <View style=styles##titleContainer>
        {token == Tez
           ? <Icons.Tezos
               size=20.
               color={theme.colors.iconMediumEmphasis}
               style=styles##icon
             />
           : <Icons.Token
               size=20.
               color={theme.colors.iconMediumEmphasis}
               style=styles##icon
             />}
        <Typography.Subtitle2>
          {token->name->React.string}
        </Typography.Subtitle2>
      </View>
      <Typography.Body1 colorStyle=`mediumEmphasis>
        {token->symbol->React.string}
      </Typography.Body1>
    </View>;
  };
};

let renderButton = (selectedToken: option(token), _hasError) =>
  <View style=styles##selectorContent>
    {selectedToken->Option.mapWithDefault(<LoadingView />, token =>
       <View style=TokenItem.styles##inner>
         <Typography.Body1> {token->symbol->React.string} </Typography.Body1>
       </View>
     )}
  </View>;

let renderItem = (token: token) => <TokenItem token />;

[@react.component]
let make =
    (
      ~selectedToken: option(PublicKeyHash.t),
      ~setSelectedToken: option(Token.t) => unit,
      ~style as styleProp=?,
      ~renderButton=renderButton,
    ) => {
  let tokens = StoreContext.Tokens.useGetAll();

  let items =
    tokens
    ->TokenRegistry.Cache.valuesToArray
    ->Array.keepMap(
        fun
        | TokenRegistry.Cache.Full(t) => t->Token->Some
        | Partial(_) => None,
      );

  let onValueChange =
    fun
    | Tez => setSelectedToken(None)
    | Token(t) => t->Some->setSelectedToken;

  items->Array.size > 0
    ? <Selector
        style=Style.(arrayOption([|Some(styles##selector), styleProp|]))
        dropdownStyle=styles##selectorDropdown
        items
        getItemKey={
          fun
          | Tez => "tez"
          | Token(t: TokenRepr.t) => (t.address :> string)
        }
        renderButton
        onValueChange
        renderItem
        selectedValueKey=?(selectedToken :> option(string))
        noneItem=Tez
        keyPopover="tokenSelector"
      />
    : React.null;
};
