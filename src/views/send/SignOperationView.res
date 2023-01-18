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

let styles = {
  open Style
  StyleSheet.create({
    "advancedOptions": style(~marginBottom=12.->dp, ()),
    "edited": style(~marginRight=5.->dp, ()),
    "timeoutError": style(~alignItems=#flexStart, ~marginTop=20.->dp, ~marginBottom=20.->dp, ()),
  })
}

type step =
  | AdvancedOptStep(option<int>)
  | SummaryStep

let makeTitle = (~custom=?, x) =>
  switch x {
  | AdvancedOptStep(_) => I18n.Label.advanced_options
  | SummaryStep => custom->Option.getWithDefault(I18n.Title.confirmation)
  }

let back = ((step, set), f) =>
  switch step {
  | AdvancedOptStep(_) => Some(() => set(_ => SummaryStep))
  | SummaryStep => f()
  }

@react.component
let make = (
  ~sender: Alias.t,
  ~signer: Account.t,
  ~state,
  ~signOpStep as (step, setStep),
  ~dryRun,
  ~secondaryButton=?,
  ~operation,
  ~sendOperation: (~operation: Protocol.batch, TaquitoAPI.Signer.intent) => Promise.t<_>,
  ~loading,
  ~icon=?,
  ~name=?,
) => {
  Js.log(__LOC__)
  Js.log(sender)
  Js.log(__LOC__)
  Js.log(signer)
  Js.log(__LOC__)
  let ((operation: Protocol.batch, dryRun), setOp) = React.useState(() => (operation, dryRun))

  let theme = ThemeContext.useTheme()

  let subtitle = switch signer.Account.kind {
  | Ledger => I18n.Expl.hardware_wallet_confirm_operation
  | Galleon
  | Encrypted
  | Unencrypted => I18n.Expl.confirm_operation
  | CustomAuth(_) => I18n.Expl.custom_auth_confirm_operation
  }

  let onAdvOptSubmit = (op, dryRun) => {
    setOp(_ => (op, dryRun))

    setStep(_ => SummaryStep)
  }

  let setAdvancedOptions = i => setStep(_ => AdvancedOptStep(i))

  let advancedOptionsDisabled =
    {
      open SigningBlock
      switch fst(state) {
      | Some(WaitForConfirm | Searching | Confirmed) => true
      | None
      | Some(Error(_)) => false
      }
    } ||
    loading

  let optionsSet = x =>
    switch x {
    | [op] => ProtocolHelper.optionsSet(op)
    | _ => None
    }

  switch step {
  | SummaryStep => <>
      <View style=FormStyles.header> {subtitle->Typography.overline1} </View>
      <OperationSummaryView.Batch
        sender
        operation
        dryRun
        editAdvancedOptions={i => setAdvancedOptions(Some(i))}
        advancedOptionsDisabled
        ?icon
        ?name
      />
      {<Buttons.RightArrowButton
        style={styles["advancedOptions"]}
        disabled=advancedOptionsDisabled
        text=I18n.Label.advanced_options
        stateIcon={optionsSet(operation.managers) == Some(true)
          ? <Icons.Edit style={styles["edited"]} size=25. color=theme.colors.iconPrimary />
          : React.null}
        onPress={_ => setAdvancedOptions(None)}
      />->ReactUtils.onlyWhen(dryRun.simulations->Array.length == 1)}
      <SigningBlock
        accountKind=signer.Account.kind
        state
        ?secondaryButton
        loading
        sendOperation={sendOperation(~operation)}
      />
    </>

  | AdvancedOptStep(index) =>
    <AdvancedOptionsView operation dryRun ?index onSubmit=onAdvOptSubmit token=None />
  }
}
