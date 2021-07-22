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

let onbStyles = FormStyles.onboarding;

type ledgerState = [
  | `Found
  | `Confirmed
  | `Denied(ErrorHandler.t)
  | `Loading
];

let styles =
  Style.(
    StyleSheet.create({
      "retry": style(~width=100.->pct, ~marginTop=20.->pt, ()),
      "content":
        style(~textAlign=`center, ~display=`flex, ~alignItems=`center, ()),
      "expl": style(~height=40.->dp, ()),
      "loading": style(~height=50.->dp, ()),
    })
  );

[@react.component]
let make = (~status, ~retry) => {
  let expl = txt =>
    <Typography.Headline
      fontSize=16.
      style=Style.(array([|styles##expl, FormStyles.section##spacing|]))>
      txt
    </Typography.Headline>;

  <>
    <Typography.Headline style=onbStyles##title>
      {let title =
         switch (status) {
         | `Found => I18n.title#hardware_wallet_confirm
         | `Loading => I18n.title#hardware_wallet_search
         | `Confirmed => I18n.title#hardware_wallet_confirmed
         | `Denied(ErrorHandler.Taquito(ReTaquitoError.LedgerInitTimeout)) =>
           I18n.title#hardware_wallet_not_found
         | `Denied(ErrorHandler.Taquito(LedgerKeyRetrieval)) =>
           I18n.title#hardware_wallet_error_app
         | `Denied(Taquito(LedgerDenied)) =>
           I18n.title#hardware_wallet_denied
         | `Denied(Taquito(LedgerNotReady)) =>
           I18n.title#hardware_wallet_not_ready
         | _ => I18n.title#hardware_wallet_error_unknown
         };
       title->React.string}
    </Typography.Headline>
    {switch (status) {
     | `Denied(err) =>
       <View style=styles##content>
         <Icons.CloseOutline
           color=Colors.error
           size=50.
           style=FormStyles.section##spacing
         />
         {err->ErrorHandler.toString->React.string->expl}
         <Buttons.SubmitPrimary
           text=I18n.btn#retry
           style=styles##retry
           onPress={_ => retry()}
         />
       </View>
     | `Confirmed =>
       <View style=styles##content>
         <Icons.CheckOutline
           color=Colors.valid
           size=50.
           style=FormStyles.section##spacing
         />
         {I18n.expl#hardware_wallet_confirmed->React.string->expl}
       </View>
     | (`Loading | `Found) as st =>
       <View style=styles##content>
         <View
           style=Style.(
             array([|FormStyles.section##spacing, styles##loading|])
           )>
           <LoadingView size=ActivityIndicator_Size.large />
         </View>
         (
           switch (st) {
           | `Found => I18n.expl#hardware_wallet_confirm->React.string
           | `Loading => I18n.expl#hardware_wallet_search->React.string
           }
         )
         ->expl
       </View>
     }}
  </>;
};
