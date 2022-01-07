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

module LedgerView = {
  type state =
    | WaitForConfirm
    | Searching
    | Confirmed
    | Error(Errors.t);

  let errorTitleExpl =
    fun
    | ReTaquitoError.LedgerNotReady as e => (
        I18n.Title.hardware_wallet_not_ready,
        e->Errors.toString,
      )

    | ReTaquitoError.LedgerInitTimeout => (
        I18n.Title.hardware_wallet_not_found,
        I18n.Form_input_error.hardware_wallet_timeout,
      )
    | ReTaquitoError.LedgerKeyRetrieval => (
        I18n.Title.hardware_wallet_error_app,
        I18n.Form_input_error.hardware_wallet_check_app,
      )
    | ReTaquitoError.LedgerDenied => (
        I18n.Title.hardware_wallet_denied,
        I18n.Expl.hardware_wallet_denied,
      )
    | e => (I18n.Title.hardware_wallet_error_unknown, e->Errors.toString);

  let styles =
    Style.(
      StyleSheet.create({
        "withSecondary": style(~marginTop=10.->dp, ()),
        "container":
          style(
            ~marginTop=20.->dp,
            ~borderRadius=4.,
            ~padding=32.->dp,
            ~minHeight=296.->dp,
            (),
          ),
      })
    );

  module Content = {
    let onbStyles = FormStyles.onboarding;

    let styles =
      Style.(
        StyleSheet.create({
          "loading": style(~height=50.->dp, ()),
          "title": style(~height=40.->dp, ()),
          "content":
            style(
              ~textAlign=`center,
              ~display=`flex,
              ~alignItems=`center,
              (),
            ),
        })
      );

    [@react.component]
    let make = (~title, ~expl, ~error=false) => {
      let theme = ThemeContext.useTheme();
      <>
        <Typography.Subtitle2
          style=Style.([|onbStyles##title, styles##title|]->array)>
          title->React.string
        </Typography.Subtitle2>
        <View style=styles##content>
          {error
             ? <Icons.CloseOutline
                 color={theme.colors.error}
                 size=50.
                 style=FormStyles.section##spacing
               />
             : <View
                 style=Style.(
                   array([|FormStyles.section##spacing, styles##loading|])
                 )>
                 <LoadingView size=ActivityIndicator_Size.large />
               </View>}
          <Typography.Body1 fontSize=16. style=FormStyles.section##spacing>
            expl->React.string
          </Typography.Body1>
        </View>
      </>;
    };
  };

  [@react.component]
  let make = (~st, ~retry, ~secondaryButton=?) => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        array([|
          style(~backgroundColor=theme.colors.stateDisabled, ()),
          styles##container,
        |])
      )>
      {switch (st) {
       | WaitForConfirm =>
         <Content
           title=I18n.Title.hardware_wallet_op_confirm
           expl=I18n.Expl.hardware_wallet_op_confirm
         />
       | Searching =>
         <Content
           title=I18n.Title.hardware_wallet_search
           expl=I18n.Expl.hardware_wallet_search
         />
       | Confirmed =>
         <Content
           title=I18n.Title.hardware_wallet_op_confirmed
           expl=I18n.Expl.hardware_wallet_op_confirmed
         />
       | Error(e) =>
         let (title, expl) = e->errorTitleExpl;
         <>
           <Content title expl error=true />
           <View style=FormStyles.verticalFormAction>
             secondaryButton->ReactUtils.opt
             <Buttons.SubmitPrimary
               style=?{secondaryButton->Option.map(_ => styles##withSecondary)}
               text=I18n.Btn.retry
               onPress={_ => retry()}
             />
           </View>
         </>;
       }}
    </View>;
  };
};

[@react.component]
let make =
    (
      ~ledgerState as (ledgerState, setLedgerState),
      ~accountKind: Account.kind,
      ~sendOperation: TaquitoAPI.Signer.intent => Promise.t(_),
      ~secondaryButton=?,
      ~loading=false,
    ) => {
  let (form, formFieldsAreValids) =
    PasswordFormView.usePasswordForm((~password) =>
      sendOperation(TaquitoAPI.Signer.Password(password))
    );

  let onSubmit = () =>
    switch (accountKind) {
    | Ledger =>
      setLedgerState(_ => LedgerView.Searching->Some);
      sendOperation(
        TaquitoAPI.Signer.LedgerCallback(
          () => setLedgerState(_ => LedgerView.WaitForConfirm->Some),
        ),
      )
      ->Promise.getError(e => setLedgerState(_ => Error(e)->Some));
    | Encrypted
    | Unencrypted => form.submit()
    };

  let showPasswordForm =
    switch (accountKind) {
    | Ledger => false
    | Encrypted
    | Unencrypted => true
    };

  let submitText =
    switch (accountKind) {
    | Ledger => I18n.Btn.continue
    | Encrypted
    | Unencrypted => I18n.Btn.confirm
    };

  let submitDisabled = {
    !formFieldsAreValids && accountKind != Ledger;
  };

  <>
    {<PasswordFormView.PasswordField form />
     ->ReactUtils.onlyWhen(showPasswordForm)}
    {switch (ledgerState) {
     | Some(_) => React.null
     | None =>
       <View style=FormStyles.verticalFormAction>
         secondaryButton->ReactUtils.opt
         <Buttons.SubmitPrimary
           style=?{
             secondaryButton->Option.map(_ => LedgerView.styles##withSecondary)
           }
           text=submitText
           onPress={_ => onSubmit()}
           loading
           disabled=submitDisabled
           disabledLook=submitDisabled
         />
       </View>
     }}
    {ledgerState->ReactUtils.mapOpt(st =>
       <LedgerView st ?secondaryButton retry=onSubmit />
     )}
  </>;
};
