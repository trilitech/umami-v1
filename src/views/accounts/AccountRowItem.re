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
      "inner": style(~marginRight=10.->dp, ~marginLeft=14.->dp, ()),
      "actionButtons":
        style(
          ~alignSelf=`flexEnd,
          ~flexDirection=`row,
          ~flex=1.,
          ~marginBottom=6.->dp,
          (),
        ),
      "button": style(~marginRight=4.->dp, ()),
      "actionContainer": style(~marginRight=24.->dp, ()),
    })
  );

[@react.component]
let make = (~account: Account.t, ~token: option(Token.t)=?) => {
  let delegateRequest = StoreContext.Delegate.useLoad(account.address);
  let addToast = LogsContext.useToast();

  let balanceRequest = StoreContext.Balance.useLoad(account.address);

  let zeroTez =
    switch (balanceRequest->ApiRequest.getDoneOk) {
    | None => true
    | Some(balance) => balance == Tez.zero
    };

  <RowItem.Bordered height=90.>
    <View style=styles##inner> <AccountInfo account ?token /> </View>
    <View style=styles##actionButtons>
      <ClipboardButton
        copied=I18n.Log.address
        tooltipKey=(account.address :> string)
        addToast
        data=(account.address :> string)
        style=styles##button
      />
      <QrButton
        tooltipKey=(account.address :> string)
        account={account->Account.toAlias}
        style=styles##button
      />
    </View>
    {delegateRequest
     ->ApiRequest.mapWithDefault(React.null, delegate => {
         <View style=styles##actionContainer>
           <DelegateButton
             zeroTez
             action={
               delegate->Option.mapWithDefault(
                 Delegate.Create(account, false), delegate =>
                 Delegate.Edit(account, delegate)
               )
             }
           />
         </View>
       })
     ->ReactUtils.onlyWhen(token == None)}
  </RowItem.Bordered>;
};
