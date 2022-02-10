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
      ~subtitle=?,
      ~source: Account.t,
      ~ledgerState: (option(SigningBlock.LedgerView.state), _),
      ~signOpStep as (step, setStep),
      ~dryRun,
      ~secondaryButton=?,
      ~operation,
      ~sendOperation:
         (~operation: Protocol.batch, TaquitoAPI.Signer.intent) =>
         Promise.t(_),
      ~loading,
    ) => {
  let ((operation: Protocol.batch, dryRun), setOp) =
    React.useState(() => (operation, dryRun));

  let theme = ThemeContext.useTheme();

  let subtitle =
    subtitle->Option.map(((s, hs)) =>
      switch (source.Account.kind) {
      | Ledger => hs
      | Encrypted
      | Unencrypted => s
      }
    );

  let onAdvOptSubmit = (op, dryRun) => {
    setOp(_ => (op, dryRun));

    setStep(_ => SummaryStep);
  };

  let setAdvancedOptions = i => setStep(_ => AdvancedOptStep(i));

  let advancedOptionsDisabled =
    (
      switch (fst(ledgerState)) {
      | None
      | Some(Error(_)) => false
      | Some(WaitForConfirm | Searching | Confirmed) => true
      }
    )
    || loading;

  let optionsSet =
    fun
    | [|op|] => ProtocolHelper.optionsSet(op)
    | _ => None;

  switch (step) {
  | SummaryStep =>
    <>
      {subtitle->ReactUtils.mapOpt(s =>
         <View style=FormStyles.header>
           <Typography.Overline1> s->React.string </Typography.Overline1>
         </View>
       )}
      {switch (operation.managers) {
       | [|Delegation(delegation)|] =>
         <OperationSummaryView.Delegate
           source={operation.source}
           delegation
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
        ledgerState
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
