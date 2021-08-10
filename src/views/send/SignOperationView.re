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
      "timeoutError":
        style(
          ~alignItems=`flexStart,
          ~marginTop=20.->dp,
          ~marginBottom=20.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~title=?,
      ~subtitle=?,
      ~source: PublicKeyHash.t,
      ~ledgerState,
      ~children,
      ~sendOperation: TaquitoAPI.Signer.intent => Future.t(Result.t(_)),
      ~loading,
    ) => {
  let isLedger = StoreContext.Accounts.useIsLedger(source);
  <>
    {title->ReactUtils.mapOpt(title =>
       <View style=FormStyles.header>
         <Typography.Headline> title->React.string </Typography.Headline>
         {subtitle->ReactUtils.mapOpt(((subtitle, hw_subtitle)) =>
            <Typography.Overline1 style=FormStyles.subtitle>
              {isLedger ? hw_subtitle : subtitle}->React.string
            </Typography.Overline1>
          )}
       </View>
     )}
    children
    <SigningBlock isLedger ledgerState loading sendOperation />
  </>;
};
