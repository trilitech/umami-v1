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
open Let;

type state =
  | WaitForConfirm
  | Searching
  | Confirmed
  | Error(Errors.t);

module SignerView = {
  let onbStyles = FormStyles.onboarding;

  module Container = {
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

    [@react.component]
    let make = (~children) => {
      let theme = ThemeContext.useTheme();
      <View
        style=Style.(
          array([|
            style(~backgroundColor=theme.colors.stateDisabled, ()),
            styles##container,
          |])
        )>
        children
      </View>;
    };
  };

  let styles =
    Style.(
      StyleSheet.create({
        "loading": style(~height=50.->dp, ()),
        "title": style(~height=40.->dp, ()),
        "content":
          style(~textAlign=`center, ~display=`flex, ~alignItems=`center, ()),
      })
    );

  [@react.component]
  let make = (~st, ~title, ~expl) => {
    let theme = ThemeContext.useTheme();

    <>
      <Typography.Subtitle2
        style=Style.([|onbStyles##title, styles##title|]->array)>
        title->React.string
      </Typography.Subtitle2>
      <View style=styles##content>
        {switch (st) {
         | Error(_) =>
           <Icons.CloseOutline
             color={theme.colors.error}
             size=50.
             style=FormStyles.section##spacing
           />
         | Confirmed =>
           <Icons.CheckOutline
             color={theme.colors.valid}
             size=50.
             style=FormStyles.section##spacing
           />

         | Searching
         | WaitForConfirm =>
           <View
             style=Style.(
               array([|FormStyles.section##spacing, styles##loading|])
             )>
             <LoadingView size=ActivityIndicator_Size.large />
           </View>
         }}
        <Typography.Body1 fontSize=16. style=FormStyles.section##spacing>
          expl->React.string
        </Typography.Body1>
      </View>
    </>;
  };
};

module CustomAuthView = {
  let caStyles =
    Style.(StyleSheet.create({"buttons": style(~marginTop=20.->dp, ())}));

  open SignerView;
  [@react.component]
  let make = (~st, ~retry, ~loading, ~submit, ~provider) => {
    let () = ReCustomAuthUtils.useDeeplinkHandler();

    <Container>
      {switch (st) {
       | WaitForConfirm
       | Searching =>
         <SignerView
           st
           title=I18n.Title.custom_auth_waiting_auth
           expl={I18n.Expl.custom_auth_sign(
             provider->ReCustomAuth.getProviderName,
           )}
         />
       | Confirmed =>
         <>
           <SignerView
             st
             title=I18n.Title.custom_auth_success
             expl=I18n.Expl.custom_auth_success
           />
           <View style=FormStyles.verticalFormAction>
             <Buttons.SubmitPrimary
               loading
               text=I18n.Btn.confirm
               onPress={_ => submit()}
             />
           </View>
         </>
       | Error(e) =>
         let (title, expl) = (
           I18n.Title.custom_auth_failed,
           e->Errors.toString,
         );
         <>
           <SignerView title expl st />
           <View style=FormStyles.verticalFormAction>
             <Buttons.SubmitPrimary
               text=I18n.Btn.retry
               onPress={_ => retry()}
             />
           </View>
         </>;
       }}
    </Container>;
  };
};

module LedgerView = {
  open SignerView;

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

  [@react.component]
  let make = (~st, ~retry, ~secondaryButton=?) => {
    <Container>
      {switch (st) {
       | WaitForConfirm =>
         <SignerView
           st
           title=I18n.Title.hardware_wallet_op_confirm
           expl=I18n.Expl.hardware_wallet_op_confirm
         />
       | Searching =>
         <SignerView
           st
           title=I18n.Title.hardware_wallet_search
           expl=I18n.Expl.hardware_wallet_search
         />
       | Confirmed =>
         <SignerView
           st
           title=I18n.Title.hardware_wallet_op_confirmed
           expl=I18n.Expl.hardware_wallet_op_confirmed
         />
       | Error(e) =>
         let (title, expl) = e->errorTitleExpl;
         <>
           <SignerView title expl st />
           <View style=FormStyles.verticalFormAction>
             secondaryButton->ReactUtils.opt
             <Buttons.SubmitPrimary
               style=?{
                 secondaryButton->Option.map(_ =>
                   SignerView.Container.styles##withSecondary
                 )
               }
               text=I18n.Btn.retry
               onPress={_ => retry()}
             />
           </View>
         </>;
       }}
    </Container>;
  };
};

[@react.component]
let make =
    (
      ~state as (st, setState),
      ~accountKind: Account.kind,
      ~sendOperation: TaquitoAPI.Signer.intent => Promise.t(_),
      ~secondaryButton=?,
      ~loading=false,
    ) => {
  let (form, formFieldsAreValids) =
    PasswordFormView.usePasswordForm((~password) =>
      sendOperation(TaquitoAPI.Signer.Password(password))
    );

  let (customAuthSigner, setCustomAuthSigner) = React.useState(() => None);

  let signCustomAuth = infos =>
    Promise.async(() => {
      setState(_ => Some(WaitForConfirm));
      let%AwaitMap signer =
        ReCustomAuthSigner.create(infos)
        ->Promise.tapError(e => {setState(_ => Some(Error(e)))});

      setCustomAuthSigner(_ => Some(signer));
      setState(_ => Some(Confirmed));
    });

  let onSubmit = () =>
    switch (accountKind, customAuthSigner) {
    | (Ledger, _) =>
      sendOperation(
        TaquitoAPI.Signer.LedgerCallback(
          () => setState(_ => Some(WaitForConfirm)),
        ),
      )
      ->Promise.getError(e => setState(_ => Some(Error(e))))
    | (Encrypted | Unencrypted, _) => form.submit()
    | (CustomAuth(_), Some(signer)) =>
      sendOperation(TaquitoAPI.Signer.CustomAuthSigner(signer))
      ->Promise.getError(e => setState(_ => Some(Error(e))))
    | (CustomAuth(infos), None) => signCustomAuth(infos)
    };

  let showPasswordForm =
    switch (accountKind) {
    | Ledger
    | CustomAuth(_) => false
    | Encrypted
    | Unencrypted => true
    };

  let submitText =
    switch (accountKind) {
    | Ledger
    | CustomAuth(_) => I18n.Btn.continue
    | Encrypted
    | Unencrypted => I18n.Btn.confirm
    };

  let submitDisabled = {
    !formFieldsAreValids && showPasswordForm;
  };

  <>
    {<PasswordFormView.PasswordField form />
     ->ReactUtils.onlyWhen(showPasswordForm)}
    {switch (st) {
     | Some(_) => React.null
     | None =>
       <View style=FormStyles.verticalFormAction>
         secondaryButton->ReactUtils.opt
         <Buttons.SubmitPrimary
           style=?{
             secondaryButton->Option.map(_ =>
               SignerView.Container.styles##withSecondary
             )
           }
           text=submitText
           onPress={_ => onSubmit()}
           loading
           disabled=submitDisabled
           disabledLook=submitDisabled
         />
       </View>
     }}
    {st->ReactUtils.mapOpt(st =>
       switch (accountKind) {
       | Ledger => <LedgerView st ?secondaryButton retry=onSubmit />
       | CustomAuth(infos) =>
         <CustomAuthView
           st
           submit=onSubmit
           loading
           provider={infos.provider}
           retry={() => signCustomAuth(infos)}
         />
       | Encrypted
       | Unencrypted => React.null
       }
     )}
  </>;
};
