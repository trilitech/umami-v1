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

module StateLenses = [%lenses
  type state = {
    node: string,
    mezos: string,
  }
];
module NetworkInfoCreateForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "buttonsRow":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "container": style(~flexDirection=`column, ()),
      "notFirstItem": style(~marginTop=24.->dp, ()),
      "content":
        style(
          ~borderRadius=4.,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingVertical=2.->dp,
          ~paddingLeft=16.->dp,
          ~paddingRight=2.->dp,
          (),
        ),
      "title": style(~marginBottom=6.->dp, ~textAlign=`center, ()),
      "overline": style(~marginBottom=24.->dp, ~textAlign=`center, ()),
    })
  );

module Item = {
  [@react.component]
  let make = (~style as styleArg=?, ~value, ~label) => {
    let theme = ThemeContext.useTheme();
    let addToast = LogsContext.useToast();

    let backStyle =
      Style.(
        style(
          ~color=theme.colors.textMediumEmphasis,
          ~backgroundColor=theme.colors.stateDisabled,
          ~borderWidth=0.,
          (),
        )
      );

    <View style={Style.arrayOption([|styleArg, styles##container->Some|])}>
      <FormLabel label style=FormGroupTextInput.styles##label />
      <View style={Style.array([|styles##content, backStyle|])}>
        <Typography.Body1> value->React.string </Typography.Body1>
        <ClipboardButton size=40. copied=label addToast data=value />
      </View>
    </View>;
  };
};

[@react.component]
let make = (~network: Network.network, ~closeAction) => {
  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.headerWithoutMarginBottom>
      {network.name}->React.string
    </Typography.Headline>
    <Item label=I18n.label#custom_network_node_url value={network.endpoint} />
    <Item
      style=styles##notFirstItem
      label=I18n.label#custom_network_mezos_url
      value={network.explorer}
    />
  </ModalFormView>;
};
