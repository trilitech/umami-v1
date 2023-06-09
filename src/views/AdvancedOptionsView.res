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

module StateLenses = %lenses(
  type state = {
    fee: string,
    gasLimit: string,
    storageLimit: string,
  }
)

type validState = {
  fee: option<Tez.t>,
  gasLimit: option<int>,
  storageLimit: option<int>,
}

let extractValidState = (state: StateLenses.state): validState => {
  fee: state.fee->Tez.fromString,
  gasLimit: state.gasLimit->Int.fromString,
  storageLimit: state.storageLimit->Int.fromString,
}

module Form = {
  include ReForm.Make(StateLenses)

  let fromDryRun = (dr: Protocol.Simulation.results, showLimits, index) => {
    open StateLenses
    {
      fee: dr.simulations
      ->Array.get(index)
      ->Option.mapWithDefault("", sim => sim.fee->Tez.toString),
      gasLimit: showLimits
        ? dr.simulations
          ->Array.get(index)
          ->Option.mapWithDefault("", sim => sim.gasLimit->Int.toString)
        : "",
      storageLimit: showLimits
        ? dr.simulations
          ->Array.get(index)
          ->Option.mapWithDefault("", sim => sim.storageLimit->Int.toString)
        : "",
    }
  }

  let use = (showLimits, dryRun, index, onSubmit) =>
    use(
      ~schema={
        open Validation
        Schema(custom(values => {
            open FormUtils
            emptyOr(isValidTezAmount, values.fee)
          }, Fee) + custom(
            values => FormUtils.isValidInt(values.gasLimit),
            GasLimit,
          ) + custom(values => FormUtils.isValidInt(values.storageLimit), StorageLimit))
      },
      ~onSubmit=f => {
        onSubmit(f)
        None
      },
      ~initialState=dryRun->fromDryRun(showLimits, index),
      ~i18n=FormUtils.i18n,
      (),
    )
}

let styles = {
  open Style
  StyleSheet.create({
    "formRowInputs": style(~flexDirection=#row, ~justifyContent=#center, ()),
    "formRowInputsSeparator": style(~width=13.->dp, ()),
    "formRowInput": style(
      ~flexGrow=1.,
      ~flexShrink=1.,
      ~flexBasis=0.->dp,
      ~marginVertical=5.->dp,
      (),
    ),
  })
}

let mapOptions = (values: validState, o) => {
  let fallback = (o1, o2) => Option.firstSome(list{o1, o2})

  open ProtocolOptions
  {
    gasLimit: fallback(values.gasLimit, o.gasLimit),
    fee: fallback(values.fee, o.fee),
    storageLimit: fallback(values.storageLimit, o.storageLimit),
  }
}

let updateOperation = (index, values: StateLenses.state, managers: array<Protocol.manager>) => {
  let values = values->extractValidState
  managers->Array.mapWithIndex((i, op) =>
    if index == i {
      switch op {
      | Transfer(t) =>
        let options = mapOptions(values, t.options)
        {...t, options: options}->Protocol.Transfer
      | Delegation(d) =>
        let options = mapOptions(values, d.options)
        {...d, options: options}->Protocol.Delegation
      | Origination(o) =>
        let options = mapOptions(values, o.options)
        {...o, options: options}->Protocol.Origination
      }
    } else {
      op
    }
  )
}

@react.component
let make = (
  ~signer: Account.t,
  ~operations: array<Protocol.manager>,
  ~dryRun,
  ~index=0,
  ~token,
  ~onSubmit,
) => {
  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let showLimits =
    token != None ||
      operations->Array.get(index)->Option.mapWithDefault(false, ProtocolHelper.isContractCall)

  let form = Form.use(showLimits, dryRun, index, ({state}) => {
    let managers = updateOperation(index, state.values, operations)
    sendOperationSimulate(signer, managers)->Promise.get(x =>
      switch x {
      | Ok(dr) =>
        let managers = updateOperation(index, dr->Form.fromDryRun(showLimits, index), managers)
        onSubmit(managers, dr)
      | Error(_) => ()
      }
    )
  })

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <View>
    <View style={styles["formRowInputs"]}>
      <FormGroupCurrencyInput
        label=I18n.Label.fee
        value=form.values.fee
        handleChange={fee => form.handleChange(Fee, fee)}
        error={form.getFieldError(Field(Fee))}
        style={styles["formRowInput"]}
        decoration=FormGroupCurrencyInput.tezDecoration
      />
      {<>
        <View style={styles["formRowInputsSeparator"]} />
        <FormGroupTextInput
          label=I18n.Label.gas_limit
          value=form.values.gasLimit
          handleChange={form.handleChange(GasLimit)}
          error={form.getFieldError(Field(GasLimit))}
          style={styles["formRowInput"]}
        />
      </>->ReactUtils.onlyWhen(showLimits)}
      {<>
        <View style={styles["formRowInputsSeparator"]} />
        <FormGroupTextInput
          label=I18n.Label.storage_limit
          value=form.values.storageLimit
          handleChange={form.handleChange(StorageLimit)}
          error={form.getFieldError(Field(StorageLimit))}
          style={styles["formRowInput"]}
        />
      </>->ReactUtils.onlyWhen(showLimits)}
    </View>
    <Buttons.SubmitPrimary
      text=I18n.Btn.update
      loading={operationSimulateRequest->ApiRequest.isLoading}
      onPress={_ => form.submit()}
      disabledLook={!formFieldsAreValids}
    />
  </View>
}
