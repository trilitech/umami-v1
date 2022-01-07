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
      "container": style(~alignItems=`center, ()),
      "title": style(~marginBottom=6.->dp, ()),
      "body": style(~marginBottom=30.->dp, ~textAlign=`center, ()),
      "hashTitle": style(~marginBottom=2.->dp, ()),
      "addressContainer":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
      "address": style(~maxWidth=80.->dp, ~overflow=`hidden, ()),
    })
  );

[@react.component]
let make = (~hash, ~onPressCancel, ~submitText=I18n.Btn.ok) => {
  let addToast = LogsContext.useToast();

  <View style=styles##container>
    <Typography.Headline style=styles##title>
      I18n.Title.operation_submitted->React.string
    </Typography.Headline>
    <Typography.Body2 colorStyle=`mediumEmphasis style=styles##body>
      I18n.Expl.operation->React.string
    </Typography.Body2>
    <Typography.Overline2 style=styles##hashTitle>
      I18n.operation_hash->React.string
    </Typography.Overline2>
    <View style=styles##addressContainer>
      <Typography.Address numberOfLines=1 style=styles##address>
        hash->React.string
      </Typography.Address>
      <ClipboardButton
        tooltipKey="OperationSubmitted"
        copied=I18n.Log.address
        addToast
        data=hash
      />
    </View>
    <View style=FormStyles.formAction>
      <Buttons.FormPrimary text=submitText onPress=onPressCancel />
    </View>
  </View>;
};
