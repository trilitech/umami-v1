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

type step =
  | AdvancedOptStep(option(int))
  | SummaryStep;

[@react.component]
let make =
    (
      ~subtitle=?,
      ~source: Account.t,
      ~ledgerState,
      ~signOpStep as (step, setStep),
      ~dryRun,
      ~operation,
      ~sendOperation:
         (~operation: Operation.t, TaquitoAPI.Signer.intent) =>
         Future.t(Result.t(_)),
      ~loading,
    ) => {
  let ((operation: Operation.t, dryRun), setOp) =
    React.useState(() => (operation, dryRun));

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

  switch (step) {
  | SummaryStep =>
    <>
      {subtitle->ReactUtils.mapOpt(s =>
         <View style=FormStyles.header>
           <Typography.Overline1> s->React.string </Typography.Overline1>
         </View>
       )}
      {switch (operation) {
       | Delegation(delegation) =>
         <OperationSummaryView.Delegate delegation dryRun />
       | Transaction(transfer) =>
         <OperationSummaryView.Transactions
           transfer
           dryRun
           editAdvancedOptions={i => setAdvancedOptions(Some(i))}
         />
       }}
      <Buttons.FormSecondary
        text=I18n.label#advanced_options
        onPress={_ => setAdvancedOptions(None)}
      />
      <SigningBlock
        accountKind={source.Account.kind}
        ledgerState
        loading
        sendOperation={sendOperation(~operation)}
      />
    </>

  | AdvancedOptStep(index) =>
    <AdvancedOptionsView
      operation
      dryRun
      index
      onSubmit=onAdvOptSubmit
      token=None
    />
  };
};
