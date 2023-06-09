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

module StateLenses = %lenses(
  type state = {
    name: string,
    address: string,
  }
)
module AccountCreateForm = ReForm.Make(StateLenses)

type action =
  | Create
  | Edit(Alias.t)

let addressExistsCheck = (aliases, values: StateLenses.state): ReSchema.fieldState => {
  let alias =
    values.address
    ->PublicKeyHash.build
    ->Result.mapWithDefault(None, address => aliases->PublicKeyHash.Map.get(address))
  switch alias {
  | None => Valid
  | Some(a: Alias.t) => Error(I18n.Form_input_error.key_already_registered(a.name))
  }
}

let formCheckExists = (aliases, values: StateLenses.state) =>
  AliasHelpers.formCheckExists(aliases, values.name)

let isEditMode = x =>
  switch x {
  | Edit(_) => true
  | _ => false
  }

@react.component
let make = (~initAddress: option<PublicKeyHash.t>=?, ~action: action, ~closeAction) => {
  let (createAliasRequest, createAlias) = StoreContext.Aliases.useCreate()
  let (updateAliasRequest, updateAlias) = StoreContext.Aliases.useUpdate()

  let aliasesRequest = StoreContext.Aliases.useRequest()
  let aliases = aliasesRequest->ApiRequest.getDoneOk->Option.getWithDefault(PublicKeyHash.Map.empty)

  let form: AccountCreateForm.api = AccountCreateForm.use(
    ~schema={
      open AccountCreateForm.Validation
      Schema(
        nonEmpty(Name) +
        custom(values => values.address->FormUtils.checkAddress, Address) +
        custom(
          values => action->isEditMode ? Valid : addressExistsCheck(aliases, values),
          Address,
        ) +
        custom(formCheckExists(aliases), Name),
      )
    },
    ~onSubmit=({state}) => {
      switch action {
      | Create =>
        createAlias((
          state.values.name,
          state.values.address->PublicKeyHash.build->Result.getExn,
        ))->Promise.getOk(_ => closeAction())
      | Edit(account) =>
        updateAlias({new_name: state.values.name, old_name: account.name})->Promise.getOk(_ =>
          closeAction()
        )
      }

      None
    },
    ~initialState=switch action {
    | Create => {
        name: "",
        address: (initAddress :> option<string>)->Option.getWithDefault(""),
      }
    | Edit(account) => {
        name: account.name,
        address: (account.address :> string),
      }
    },
    ~i18n=FormUtils.i18n,
    (),
  )

  let onSubmit = _ => form.submit()

  let loading = createAliasRequest->ApiRequest.isLoading || updateAliasRequest->ApiRequest.isLoading

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <Typography.Headline style=FormStyles.header>
      {switch action {
      | Create => I18n.Title.add_contact
      | Edit(_) => I18n.Title.update_contact
      }->React.string}
    </Typography.Headline>
    <FormGroupTextInput
      label=I18n.Label.add_contact_name
      value=form.values.name
      placeholder=I18n.Input_placeholder.add_contacts_name
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
      onSubmitEditing=onSubmit
    />
    <FormGroupTextInput
      label=I18n.Label.add_contact_address
      value=form.values.address
      placeholder=I18n.Input_placeholder.add_contacts_tz
      handleChange={form.handleChange(Address)}
      error={form.getFieldError(Field(Address))}
      onSubmitEditing=onSubmit
      disabled=?{switch action {
      | Create => None
      | Edit(_) => Some(true)
      }}
      clearButton=true
    />
    <Buttons.SubmitPrimary
      text={switch action {
      | Create => I18n.Btn.add
      | Edit(_) => I18n.Btn.update
      }}
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>
}
