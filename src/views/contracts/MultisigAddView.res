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

module StateLenses = %lenses(type state = {address: string})

module MultisigAddForm = ReForm.Make(StateLenses)

type step =
  | Address
  | CheckContract(Multisig.t)

module FormAddress = {
  @react.component
  let make = (~form: MultisigAddForm.api) =>
    <ContractAddView.FormAddress
      handleChange={form.handleChange(Address)}
      error={list{
        form.formState->FormUtils.getFormStateError,
        form.getFieldError(Field(Address)),
      }->Option.firstSome}
    />
}

module Address = {
  @react.component
  let make = (~setStep, ~closeAction) => {
    let config = ConfigContext.useContent()
    let (loading, setLoading) = React.useState(() => false)

    let fetchMultisig = pkh => {
      setLoading(_ => true)
      Multisig.API.getOneFromNetwork(config.network, pkh)
    }

    let onSubmit = ({state}: MultisigAddForm.onSubmitAPI) => {
      open MultisigAddForm
      state.values.address
      ->PublicKeyHash.buildContract
      ->Result.getExn
      ->fetchMultisig
      ->Promise.tapError(_ => setLoading(_ => false))
      ->Promise.getOk(multisig => setStep(_ => CheckContract(multisig)))
      None
    }

    let form: MultisigAddForm.api = MultisigAddForm.use(
      ~schema={
        open MultisigAddForm.Validation
        Schema(custom(state =>
            switch PublicKeyHash.buildContract(state.address) {
            | Error(_) => Error(I18n.Form_input_error.invalid_key_hash)
            | Ok(_) => Valid
            }
          , Address))
      },
      ~onSubmit,
      ~initialState={address: ""},
      ~i18n=FormUtils.i18n,
      (),
    )

    let onSubmit = _ => form.submit()

    <ModalFormView closing=ModalFormView.Close(closeAction)>
      <ContractDetailsView.Title text=I18n.Title.add_token />
      <Tag content="multisig" />
      <ContractDetailsView.Overline text=I18n.add_token_format_contract_sentence />
      <FormAddress form />
      <Buttons.SubmitPrimary
        text=I18n.Btn.save_and_register
        onPress=onSubmit
        loading
        style=FormStyles.formSubmit
        disabledLook={FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)}
      />
    </ModalFormView>
  }
}

@react.component
let make = (~closeAction) => {
  let (step, setStep) = React.useState(_ => Address)
  switch step {
  | Address => <Address setStep closeAction />
  | CheckContract(multisig) => <ContractDetailsView.Multisig multisig closeAction />
  }
}
