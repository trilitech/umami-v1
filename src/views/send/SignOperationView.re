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

module LedgerBlock = {
  type state =
    | WaitForConfirm
    | Searching
    | Confirmed
    | Error(ErrorHandler.t);

  let errorTitleExpl =
    fun
    | ErrorHandler.Taquito(LedgerNotReady) as e => (
        I18n.title#hardware_wallet_not_ready,
        e->ErrorHandler.toString,
      )

    | ErrorHandler.Taquito(LedgerInitTimeout) => (
        I18n.title#hardware_wallet_not_found,
        I18n.form_input_error#hardware_wallet_timeout,
      )
    | ErrorHandler.Taquito(LedgerKeyRetrieval) => (
        I18n.title#hardware_wallet_error_app,
        I18n.form_input_error#hardware_wallet_check_app,
      )
    | ErrorHandler.Taquito(LedgerDenied) => (
        I18n.title#hardware_wallet_denied,
        I18n.expl#hardware_wallet_denied,
      )
    | e => (
        I18n.title#hardware_wallet_error_unknown,
        e->ErrorHandler.toString,
      );

  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~marginTop=20.->dp,
            ~borderRadius=4.,
            ~padding=32.->dp,
            ~minHeight=282.->dp,
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
      <>
        <Typography.Subtitle2 style=onbStyles##title>
          title->React.string
        </Typography.Subtitle2>
        <View style=styles##content>
          {error
             ? <Icons.CloseOutline
                 color=Colors.error
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
  let make = (~st, ~retry) => {
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
           title=I18n.title#hardware_wallet_op_confirm
           expl=I18n.expl#hardware_wallet_op_confirm
         />
       | Searching =>
         <Content
           title=I18n.title#hardware_wallet_search
           expl=I18n.expl#hardware_wallet_search
         />
       | Confirmed =>
         <Content
           title=I18n.title#hardware_wallet_op_confirmed
           expl=I18n.expl#hardware_wallet_op_confirmed
         />
       | Error(e) =>
         let (title, expl) = e->errorTitleExpl;
         <>
           <Content title expl error=true />
           <View style=FormStyles.verticalFormAction>
             <Buttons.SubmitPrimary
               text=I18n.btn#retry
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
      ~title=?,
      ~subtitle=?,
      ~source: PublicKeyHash.t,
      ~ledgerState as (ledgerState, setLedgerState),
      ~children,
      ~sendOperation: TaquitoAPI.Signer.intent => Future.t(Result.t(_)),
      ~loading=false,
    ) => {
  let account: Account.t =
    StoreContext.Accounts.useGetFromAddress(source)->Option.getExn;

  let secrets = StoreContext.Secrets.useGetAll();
  let isLedger = account.address->WalletAPI.Accounts.isLedger(secrets);

  let (form, formFieldsAreValids) =
    PasswordFormView.usePasswordForm((~password) =>
      sendOperation(TaquitoAPI.Signer.Password(password))
    );

  let onSubmit = () =>
    if (isLedger) {
      setLedgerState(_ => LedgerBlock.Searching->Some);
      sendOperation(
        TaquitoAPI.Signer.LedgerCallback(
          () => setLedgerState(_ => LedgerBlock.WaitForConfirm->Some),
        ),
      )
      ->Future.tapError(e => setLedgerState(_ => Error(e)->Some))
      ->ignore;
    } else {
      form.submit();
    };

  React.useEffect0(() => {
    if (isLedger) {
      onSubmit();
    };
    None;
  });

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
    {{
       <>
         <PasswordFormView.PasswordField form />
         <View style=FormStyles.verticalFormAction>
           <Buttons.SubmitPrimary
             text=I18n.btn#confirm
             onPress={_ => onSubmit()}
             loading
             disabledLook={!isLedger && !formFieldsAreValids}
           />
         </View>
       </>;
     }
     ->ReactUtils.onlyWhen(!isLedger)}
    {ledgerState->ReactUtils.mapOpt(st => <LedgerBlock st retry=onSubmit />)}
  </>;
};
