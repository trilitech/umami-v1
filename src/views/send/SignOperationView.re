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
      "advancedOptions": style(~marginBottom=12.->dp, ()),
      "edited": style(~marginRight=5.->dp, ()),
      "timeoutError":
        style(
          ~alignItems=`flexStart,
          ~marginTop=20.->dp,
          ~marginBottom=20.->dp,
          (),
        ),
    })
  );

type step =
  | AdvancedOptStep(option(int))
  | SummaryStep;

let makeTitle = (~custom=?) =>
  fun
  | AdvancedOptStep(_) => I18n.Label.advanced_options
  | SummaryStep => custom->Option.getWithDefault(I18n.Title.confirmation);

let back = ((step, set), f) =>
  switch (step) {
  | AdvancedOptStep(_) => Some(() => set(_ => SummaryStep))
  | SummaryStep => f()
  };

[@react.component]
let make =
    (
      ~source: Account.t,
      ~state,
      ~signOpStep as (step, setStep),
      ~dryRun,
      ~secondaryButton=?,
      ~operation,
      ~sendOperation:
         (~operation: Operation.t, TaquitoAPI.Signer.intent) => Promise.t(_),
      ~loading,
    ) => {
  let ((operation: Operation.t, dryRun), setOp) =
    React.useState(() => (operation, dryRun));

  let theme = ThemeContext.useTheme();

  let subtitle =
    switch (source.Account.kind) {
    | Ledger => I18n.Expl.hardware_wallet_confirm_operation
    | Encrypted
    | Unencrypted => I18n.Expl.confirm_operation
    | CustomAuth(_) => I18n.Expl.custom_auth_confirm_operation
    };

  let onAdvOptSubmit = (op, dryRun) => {
    setOp(_ => (op, dryRun));

    setStep(_ => SummaryStep);
  };

  let setAdvancedOptions = i => setStep(_ => AdvancedOptStep(i));

  let advancedOptionsDisabled =
    SigningBlock.(
      switch (fst(state)) {
      | Some(WaitForConfirm | Searching | Confirmed) => true
      | None
      | Some(Error(_)) => false
      }
    )
    || loading;

  let optionsSet =
    fun
    | [|op|] => Protocol.optionsSet(op)
    | _ => None;

  switch (step) {
  | SummaryStep =>
    <>
      <View style=FormStyles.header>
        <Typography.Overline1> subtitle->React.string </Typography.Overline1>
      </View>
      {switch (operation.managers) {
       | [|Delegation(delegation)|] =>
         <OperationSummaryView.Delegate
           source={operation.source}
           delegation
           dryRun
         />
       | [|Origination(origination)|] =>
         <OperationSummaryView.Originate
           source={operation.source}
           origination
           dryRun
         />
       | _ =>
         <OperationSummaryView.Transactions
           operation
           dryRun
           editAdvancedOptions={i => setAdvancedOptions(Some(i))}
           advancedOptionsDisabled
         />
       }}
      {<Buttons.RightArrowButton
         style=styles##advancedOptions
         disabled=advancedOptionsDisabled
         text=I18n.Label.advanced_options
         stateIcon={
           optionsSet(operation.managers) == Some(true)
             ? <Icons.Edit
                 style=styles##edited
                 size=25.
                 color={theme.colors.iconPrimary}
               />
             : React.null
         }
         onPress={_ => setAdvancedOptions(None)}
       />
       ->ReactUtils.onlyWhen(dryRun.simulations->Array.length == 1)}
      <SigningBlock
        accountKind={source.Account.kind}
        state
        ?secondaryButton
        loading
        sendOperation={sendOperation(~operation)}
      />
    </>

  | AdvancedOptStep(index) =>
    <AdvancedOptionsView
      operation
      dryRun
      ?index
      onSubmit=onAdvOptSubmit
      token=None
    />
  };
};
