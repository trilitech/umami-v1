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
open Delegate;
open Let;

let styles =
  Style.(
    StyleSheet.create({
      "switchCmp": style(~height=16.->dp, ~width=32.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
      "operationSummary": style(~marginBottom=20.->dp, ()),
      "deleteLoading": style(~paddingVertical=170.->dp, ()),
    })
  );

let buildTransaction = (state: DelegateForm.state) => {
  Protocol.makeDelegate(
    ~source=state.values.sender,
    ~delegate=Some(state.values.baker->PublicKeyHash.build->Result.getExn),
    (),
  );
};

type step =
  | SendStep
  | PasswordStep(Protocol.delegation, Protocol.Simulation.results)
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | PasswordStep(_, _) => "passwordstep"
  | SubmittedStep(_) => "submittedstep"
  };

module Form = {
  let build = (action: action, onSubmit) => {
    let (initAccount, initDelegate) =
      switch (action) {
      | Create(account, _) => (account, None)
      | Edit(account, delegate)
      | Delete(account, delegate) => (account, Some(delegate))
      };

    DelegateForm.use(
      ~schema={
        DelegateForm.Validation.(
          Schema(
            custom(values => values.baker->FormUtils.checkAddress, Baker)
            + custom(
                values => FormUtils.(emptyOr(isValidTezAmount, values.fee)),
                Fee,
              )
            + custom(
                values =>
                  switch (initDelegate) {
                  | Some(initDelegate) =>
                    (initDelegate :> string) == values.baker
                      ? Error(I18n.Form_input_error.change_baker) : Valid
                  | None => Valid
                  },
                Baker,
              ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let operation = buildTransaction(state);
          onSubmit(operation);

          None;
        },
      ~initialState={
        sender: initAccount,
        baker: (initDelegate :> option(string))->Option.getWithDefault(""),
        fee: "",
        forceLowFee: false,
      },
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module View = {
    open DelegateForm;

    let onAppear = (el, _) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity #= value;
        },
        delay: 50.,
        onComplete: () => (),
      });
    };

    let onExit = (el, _, removeElement) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity
          #= Js.Math.max_float(0., 1. -. value -. 0.1);
        },
        delay: 0.,
        onComplete: removeElement,
      });
    };

    [@react.component]
    let make = (~form, ~action, ~loading) => {
      let onSubmitDelegateForm = _ => {
        form.submit();
      };

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <FormGroupDelegateSelector
            label=I18n.Label.account_delegate
            value={form.values.sender.address}
            handleChange={d => form.handleChange(Sender, d)}
            error={form.getFieldError(Field(Sender))}
            disabled={
              switch (action) {
              | Create(_, fixed) => fixed
              | Edit(_)
              | Delete(_) => true
              }
            }
          />
          <FormGroupBakerSelector
            label=I18n.Label.baker
            value={form.values.baker == "" ? None : form.values.baker->Some}
            handleChange={b =>
              b->Option.getWithDefault("") |> form.handleChange(Baker)
            }
            error={form.getFieldError(Field(Baker))}
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text={
                switch (action) {
                | Create(_) => I18n.Btn.delegation_submit
                | Edit(_) => I18n.Btn.update
                | Delete(_) => I18n.Btn.confirm
                }
              }
              onPress=onSubmitDelegateForm
              loading
              disabledLook={!formFieldsAreValids}
            />
          </View>
        </ReactFlipToolkit.FlippedView>
      </>;
    };
  };
};

[@react.component]
let make = (~closeAction, ~action) => {
  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendOperation = (~operation, signingIntent) =>
    sendOperation({operation, signingIntent})
    ->Promise.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))});

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  React.useEffect0(() => {
    switch (action) {
    | Delete(account, _) =>
      let op = Protocol.makeDelegate(~source=account, ~delegate=None, ());
      sendOperationSimulate(op->Operation.Simulation.delegation)
      ->Promise.getOk(dryRun => {
          setModalStep(_ => PasswordStep(op, dryRun))
        });

    | _ => ()
    };

    None;
  });

  let form =
    Form.build(action, (op: Protocol.delegation) => {
      Promise.async(() => {
        let%AwaitMap dryRun =
          sendOperationSimulate(op->Operation.Simulation.delegation);
        setModalStep(_ => PasswordStep(op, dryRun));
      })
    });

  let (signStep, _) as signOpStep =
    React.useState(() => SignOperationView.SummaryStep);

  let title =
    switch (modalStep, action) {
    | (PasswordStep({delegate}, _), _) =>
      let summaryTitle =
        delegate == None
          ? I18n.Title.delegate_delete : I18n.Title.confirm_delegate;
      SignOperationView.makeTitle(~custom=summaryTitle, signStep)->Some;
    | (SendStep, Create(_)) => I18n.Title.delegate->Some
    | (SendStep, Edit(_)) => I18n.Title.delegate_update->Some
    | (SendStep, Delete(_)) => I18n.Title.delegate_delete->Some
    | (SubmittedStep(_), _) => None
    };

  let (signingState, _) as state = React.useState(() => None);

  let closing =
    switch (modalStep, signingState: option(SigningBlock.state)) {
    | (PasswordStep({source: {kind: Ledger}}, _), Some(WaitForConfirm)) =>
      ModalFormView.Deny(I18n.Tooltip.reject_on_ledger)
    | (
        PasswordStep({source: {kind: CustomAuth({provider})}}, _),
        Some(WaitForConfirm),
      ) =>
      ModalFormView.Deny(
        I18n.Tooltip.reject_on_provider(
          provider->CustomAuthVerifiers.getProviderName,
        ),
      )
    | _ => ModalFormView.Close(closeAction)
    };

  let back =
    SignOperationView.back(signOpStep, () =>
      switch (modalStep, action) {
      | (PasswordStep(_, _), Create(_))
      | (PasswordStep(_, _), Edit(_)) =>
        Some(() => setModalStep(_ => SendStep))
      | _ => None
      }
    );

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  let onPressCancel = _ => closeAction();

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing ?title>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch (modalStep) {
           | SubmittedStep(hash) => <SubmittedView hash onPressCancel />
           | SendStep =>
             switch (action) {
             | Delete(_) => <LoadingView style=styles##deleteLoading />
             | _ => <Form.View form action loading=loadingSimulate />
             }
           | PasswordStep(delegation, dryRun) =>
             <SignOperationView
               source={delegation.source}
               signOpStep
               dryRun
               state
               operation={Operation.delegation(delegation)}
               sendOperation
               loading
             />
           }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
