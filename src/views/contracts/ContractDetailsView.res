/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2023 Nomadic Labs, <contact@nomadic-labs.com> */
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

let styles = {
  open Style
  StyleSheet.create({
    "info": style(
      ~borderRadius=4.,
      ~maxWidth=415.->dp,
      ~minHeight=44.->dp,
      ~marginTop=4.->dp,
      ~marginBottom=8.->dp,
      ~paddingHorizontal=16.->dp,
      ~paddingVertical=12.->dp,
      (),
    ),
    "title": style(~marginBottom=6.->dp, ~textAlign=#center, ()),
    "overline": style(~marginBottom=24.->dp, ~textAlign=#center, ()),
    "tag": style(~marginBottom=6.->dp, ~alignSelf=#center, ()),
    "tagContent": style(~paddingHorizontal=12.->dp, ()),
  })
}

module Info = {
  @react.component
  let make = (~children: React.element) => {
    let theme = ThemeContext.useTheme()
    let backStyle = {
      open Style
      style(~backgroundColor=theme.colors.stateDisabled, ())
    }
    <View style={Style.array([styles["info"], backStyle])}> children </View>
  }
}

module Title = {
  @react.component
  let make = (~text: string) => {
    text->Typography.headline(~style=styles["title"])
  }
}

module Tag = {
  @react.component
  let make = (~content: string) => {
    <Tag
      fontSize=14. height=26. style={styles["tag"]} contentStyle={styles["tagContent"]} content
    />
  }
}

module Overline = {
  @react.component
  let make = (~text: string) => {
    text->Typography.overline3(~style=styles["overline"])
  }
}

module Multisig = {
  module Element = {
    @react.component
    let make = (~label, ~children) => {
      let style = {
        open Style
        style(~marginTop=16.->dp, ())
      }
      <> {label->Typography.subtitle1(~style)} children </>
    }
  }

  module SimpleTextElement = {
    @react.component
    let make = (~label, ~text) => <Element label> <Info> {text->Typography.body1} </Info> </Element>
  }

  module Address = {
    @react.component
    let make = (~address) => <SimpleTextElement label=I18n.Label.add_token_address text=address />
  }

  module Name = {
    @react.component
    let make = (~name) => <SimpleTextElement label={I18n.Title.contract_name} text=name />
  }

  module Owners = {
    @react.component
    let make = (~owners, ~shrinkedAddressDisplay=false) =>
      <Element label={I18n.Title.owners(owners->Array.length->Int.toString)}>
        {owners
        ->Array.mapWithIndex((i, owner) =>
          <OperationSummaryView.EntityInfo
            key={i->Int.toString}
            style={
              open Style
              style(~marginVertical=4.->dp, ())
            }
            address={Some(owner)}
            shrinkedAddressDisplay
          />
        )
        ->React.array}
      </Element>
  }

  module Threshold = {
    @react.component
    let make = (~threshold, ~owners) =>
      <SimpleTextElement
        label=I18n.Title.approval_threshold
        text={I18n.Label.out_of(threshold->ReBigNumber.toString, owners->Int.toString)}
      />
  }

  module MultisigEdition = {
    module StateLenses = %lenses(type state = {name: string})

    module Form = ReForm.Make(StateLenses)

    module FormName = {
      @react.component
      let make = (~form: Form.api) =>
        <FormGroupTextInput
          label=I18n.Title.contract_name
          value=form.values.name
          handleChange={form.handleChange(Name)}
          error={form.getFieldError(Field(Name))}
          placeholder=I18n.Input_placeholder.add_contract_name
        />
    }
  }

  @react.component
  let make = (~multisig: Multisig.t, ~closeAction) => {
    let (_, updateMultisig) = StoreContext.Multisig.useUpdate(false)

    let onSubmit = ({state}: MultisigEdition.Form.onSubmitAPI) => {
      updateMultisig({...multisig, alias: state.values.name})->Promise.getOk(_ => closeAction())
      None
    }

    let form: MultisigEdition.Form.api = MultisigEdition.Form.use(
      ~schema={
        let aliasesRequest = StoreContext.Aliases.useRequest()
        let aliases =
          aliasesRequest->ApiRequest.getDoneOk->Option.getWithDefault(PublicKeyHash.Map.empty)
        open MultisigEdition.Form.Validation
        let aliasCheckExists = (aliases, values: MultisigEdition.StateLenses.state) =>
          AliasHelpers.formCheckExists(aliases, values.name)
        Schema(nonEmpty(Name) + custom(aliasCheckExists(aliases), Name))
      },
      ~onSubmit,
      ~initialState={name: multisig.alias},
      ~i18n=FormUtils.i18n,
      (),
    )

    let onSubmit = _ => form.submit()

    let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

    <ModalFormView closing=ModalFormView.Close(closeAction)>
      <Title text=I18n.Title.contract_details />
      <Tag content="multisig" />
      <Address address={(multisig.address :> string)} />
      <MultisigEdition.FormName form />
      <Owners owners=multisig.signers />
      <Threshold threshold=multisig.threshold owners={Array.length(multisig.signers)} />
      <Buttons.SubmitPrimary
        text=I18n.Btn.update
        onPress=onSubmit
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </ModalFormView>
  }
}
