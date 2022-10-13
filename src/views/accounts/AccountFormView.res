/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

module Generic = {
  module StateLenses = %lenses(
    type state = {
      name: string,
      secret: string,
    }
  )
  module AccountCreateForm = ReForm.Make(StateLenses)

  let aliasCheckExists = (aliases, values: StateLenses.state) =>
    AliasHelpers.formCheckExists(aliases, values.name)

  @react.component
  let make = (
    ~init,
    ~buttonText,
    ~action: (~name: string, ~secretIndex: int) => unit,
    ~request,
    ~secret: option<Secret.derived>=?,
    ~hideSecretSelector=false,
  ) => {
    let aliasesRequest = StoreContext.Aliases.useRequest()
    let aliases =
      aliasesRequest->ApiRequest.getDoneOk->Option.getWithDefault(PublicKeyHash.Map.empty)

    let form: AccountCreateForm.api = AccountCreateForm.use(
      ~schema={
        open AccountCreateForm.Validation
        Schema(nonEmpty(Name) + custom(aliasCheckExists(aliases), Name))
      },
      ~onSubmit=({state}) => {
        action(
          ~name=state.values.name,
          ~secretIndex=state.values.secret->Js.Float.fromString->int_of_float,
        )
        None
      },
      ~initialState={
        name: init,
        secret: secret->Option.mapWithDefault("", secret => secret.index->string_of_int),
      },
      ~i18n=FormUtils.i18n,
      (),
    )

    let onSubmit = _ => form.submit()

    let loading = request->ApiRequest.isLoading

    let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

    <>
      {hideSecretSelector
        ? React.null
        : <FormGroupSecretSelector
            label=I18n.Label.account_secret
            value={form.values.secret == "" ? None : Some(form.values.secret)}
            handleChange={secret => form.handleChange(Secret, secret.Secret.index->Int.toString)}
            error={form.getFieldError(Field(Secret))}
            disabled={secret->Option.isSome}
          />}
      <FormGroupTextInput
        label=I18n.Label.account_create_name
        value=form.values.name
        handleChange={form.handleChange(Name)}
        placeholder=I18n.Input_placeholder.add_accounts_name
        error={form.getFieldError(Field(Name))}
        onSubmitEditing=onSubmit
      />
      <Buttons.SubmitPrimary
        text=buttonText
        onPress=onSubmit
        loading
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </>
  }
}

module Update = {
  @react.component
  let make = (~closeAction, ~account: Account.t) => {
    let (updateAccountRequest, updateAccount) = StoreContext.Accounts.useUpdate()

    let action = (~name as new_name, ~secretIndex as _s) =>
      updateAccount({old_name: account.name, new_name: new_name})->Promise.getOk(() =>
        closeAction()
      )

    <ModalFormView closing=ModalFormView.Close(closeAction)>
      <Typography.Headline style=FormStyles.header>
        {I18n.Title.account_update->React.string}
      </Typography.Headline>
      <Generic
        init=account.name
        buttonText=I18n.Btn.update
        request=updateAccountRequest
        action
        hideSecretSelector=true
      />
    </ModalFormView>
  }
}

module Create = {
  @react.component
  let make = (~closeAction, ~secret: Secret.derived) => {
    let theme = ThemeContext.useTheme()

    let (createDeriveRequest, deriveAccount) = StoreContext.Secrets.useDerive()

    let (status, setStatus) = React.useState(() => #Loading)

    let isLedger = secret.Secret.secret.kind == Ledger

    let (formValues, setFormValues) = React.useState(_ => None)

    let actionMnemonics = (~name, ~secretIndex, ~password) =>
      deriveAccount({
        name: name,
        index: secretIndex,
        kind: Mnemonics(password),
        timeout: None,
      })->Promise.tapOk(_ => closeAction())

    let actionLedger = (~name, secretIndex, ~ledgerMasterKey) =>
      deriveAccount({
        name: name,
        index: secretIndex,
        kind: Ledger(ledgerMasterKey),
        timeout: Some(1000),
      })->Promise.tapOk(_ => closeAction())

    let ledgerInteract = (~name, ~secretIndex, secret, ()) => {
      setStatus(_ => #Loading)
      LedgerAPI.init(~timeout=5000, ())
      ->Promise.flatMapOk(tr => {
        setStatus(_ => #Found)
        LedgerAPI.getKey(
          ~prompt=true,
          tr,
          DerivationPath.Pattern.implement(
            secret.Secret.derivationPath,
            secret.addresses->Array.length,
          ),
          secret.Secret.derivationScheme,
        )->Promise.flatMapOk(_ => {
          setStatus(_ => #Found)
          LedgerAPI.getMasterKey(~prompt=false, tr)
        })
      })
      ->Promise.flatMapOk(ledgerMasterKey => {
        setStatus(_ => #Confirmed)
        Promise.timeout(1500)->Promise.flatMapOk(() =>
          actionLedger(~name, secretIndex, ~ledgerMasterKey)
        )
      })
      ->Promise.getError(e => setStatus(_ => #Denied(e)))
    }

    let ledgerStyle = {
      open Style
      style(
        ~backgroundColor=theme.colors.stateDisabled,
        ~marginTop=20.->dp,
        ~borderRadius=4.,
        ~padding=32.->dp,
        ~minHeight=282.->dp,
        (),
      )
    }

    <ModalFormView closing=ModalFormView.Close(closeAction)>
      <Typography.Headline style=FormStyles.header>
        {I18n.Title.derive_account->React.string}
      </Typography.Headline>
      {switch formValues {
      | None =>
        <Generic
          init=""
          buttonText=I18n.Btn.add
          request=createDeriveRequest
          action={(~name, ~secretIndex) => {
            if isLedger {
              ledgerInteract(~name, ~secretIndex, secret.Secret.secret, ())
            }
            setFormValues(_ => Some((name, secretIndex)))
          }}
          secret
        />
      | Some((name, secretIndex)) if !isLedger =>
        <PasswordFormView
          loading={createDeriveRequest->ApiRequest.isLoading}
          submitPassword={actionMnemonics(~name, ~secretIndex)}
        />
      | Some((name, secretIndex)) => <>
          <Typography.Overline1 fontSize=19. style=FormStyles.headerWithoutMarginBottom>
            {I18n.Title.hardware_confirm_pkh->React.string}
          </Typography.Overline1>
          <InitLedgerView
            style=ledgerStyle
            status
            retry={ledgerInteract(~name, ~secretIndex, secret.Secret.secret)}
          />
        </>
      }}
    </ModalFormView>
  }
}
