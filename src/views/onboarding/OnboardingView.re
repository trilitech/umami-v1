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
      "container":
        style(
          ~flexDirection=`row,
          ~justifyContent=`spaceAround,
          ~paddingVertical=78.->dp,
          ~paddingHorizontal=58.->dp,
          (),
        ),
      "bigbutton": style(~flex=1., ()),
    })
  );

[@react.component]
let make = (~onClose=?) => {
  let styleNotFirst =
    Style.(array([|styles##bigbutton, style(~marginLeft=60.->dp, ())|]));
  <Page>
    <Page.Header
      right={
        // If onClose is present, then this is a modal
        switch (onClose) {
        | Some(onClose) => <CloseButton onClose />
        | None => ReasonReact.null
        }
      }>
      ReasonReact.null
    </Page.Header>
    <View style=styles##container>
      <CreateAccountBigButton style=styles##bigbutton />
      <ImportAccountBigButton style=styleNotFirst />
      <RestoreAccountBigButton style=styleNotFirst />
      <HwWalletBigButton style=styleNotFirst />
    </View>
  </Page>;
};
