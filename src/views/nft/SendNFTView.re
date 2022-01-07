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

module Form = {
  let defaultInit = () =>
    SendNFTForm.StateLenses.{recipient: FormUtils.Alias.AnyString("")};

  let use = (~initValues=?, onSubmit) => {
    SendNFTForm.use(
      ~schema={
        SendNFTForm.Validation.(
          Schema(
            custom(
              values =>
                switch (values.recipient) {
                | AnyString(_) =>
                  Error(I18n.Form_input_error.invalid_contract)
                | Valid(Alias(_)) => Valid
                | Valid(Address(_)) => Valid
                },
              Recipient,
            ),
          )
        );
      },
      ~onSubmit=
        f => {
          onSubmit(f);
          None;
        },
      ~initialState=initValues->Option.getWithDefault(defaultInit()),
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module FormGroupNFTView = {
    let styles =
      Style.(
        StyleSheet.create({
          "formGroup": style(~marginBottom=20.->dp, ()),
          "label": style(~marginBottom=6.->dp, ()),
          "input": style(~paddingHorizontal=20.->dp, ~fontWeight=`bold, ()),
        })
      );

    [@react.component]
    let make = (~nft: Token.t) => {
      <FormGroup style=styles##formGroup>
        <FormLabel
          label=I18n.Label.send_nft
          hasError=false
          style=styles##label
        />
        <View>
          <ThemedTextInput
            style=styles##input
            value={nft.alias}
            hasError=false
            onValueChange={_ => ()}
            onFocus={_ => ()}
            onBlur={_ => ()}
            onKeyPress={_ => ()}
            disabled=true
          />
        </View>
      </FormGroup>;
    };
  };

  module View = {
    open SendNFTForm;

    [@react.component]
    let make = (~sender, ~nft, ~form, ~aliases, ~loading) => {
      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <FormGroupAccountSelector
            disabled=true
            label=I18n.Label.send_sender
            value={Some(sender)}
            handleChange={_ => ()}
            error=None
          />
          <FormGroupNFTView nft />
          <FormGroupContactSelector
            label=I18n.Label.send_recipient
            filterOut={sender->Account.toAlias->Some}
            aliases
            value={form.values.recipient}
            handleChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=I18n.Btn.send_submit
              onPress={_ => form.submit()}
              loading
              disabledLook={!formFieldsAreValids}
            />
          </View>
        </ReactFlipToolkit.FlippedView>
      </>;
    };
  };
};

type step =
  | SendStep
  | SigningStep(Transfer.t, Protocol.Simulation.results)
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | SigningStep(_, _) => "signingstep"
  | SubmittedStep(_) => "submittedstep"
  };

[@react.component]
let make = (~source: Account.t, ~nft: Token.t, ~closeAction) => {
  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let sendTransfer = (~operation: Operation.t, signingIntent) => {
    sendOperation({operation, signingIntent})
    ->Promise.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))});
  };

  let (sign, _setSign) as signOpStep =
    React.useState(() => SignOperationView.SummaryStep);

  let onSubmit = ({state, _}: SendNFTForm.onSubmitAPI) => {
    let transaction = {
      Transfer.source,
      transfers: [
        {
          destination:
            state.values.recipient
            ->FormUtils.Unsafe.account
            ->FormUtils.Alias.address,
          tx_options: ProtocolOptions.makeTransferEltOptions(),
          amount: Token(TokenRepr.Unit.one, nft),
        },
      ],
      options: ProtocolOptions.makeTransferOptions(),
    };
    sendOperationSimulate(Protocol.Transaction(transaction))
    ->Promise.getOk(dryRun => {
        setModalStep(_ => SigningStep(transaction, dryRun))
      });
    ();
  };

  let form: SendNFTForm.api = Form.use(onSubmit);

  let title =
    switch (modalStep) {
    | SendStep => Some(I18n.Title.send)
    | SigningStep(_, _) => SignOperationView.makeTitle(sign)->Some
    | SubmittedStep(_) => None
    };

  let back =
    SignOperationView.back(signOpStep, () =>
      switch (modalStep) {
      | SigningStep(_, _) => Some(() => setModalStep(_ => SendStep))
      | _ => None
      }
    );

  let closing = Some(ModalFormView.Close(closeAction));

  let ledgerState = React.useState(() => None);
  let aliasesRequest = StoreContext.Aliases.useRequest();

  let aliases =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault(PublicKeyHash.Map.empty);

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView ?title back ?closing>
        {switch (modalStep) {
         | SendStep =>
           <Form.View sender=source nft form aliases loading=loadingSimulate />
         | SigningStep(transfer, dryRun) =>
           <SignOperationView
             source={transfer.source}
             ledgerState
             signOpStep
             dryRun
             subtitle=(
               I18n.Expl.confirm_operation,
               I18n.Expl.hardware_wallet_confirm_operation,
             )
             operation={Operation.transaction(transfer)}
             sendOperation={(~operation, signingIntent) =>
               sendTransfer(~operation, signingIntent)
             }
             loading
           />
         | SubmittedStep(hash) =>
           let onPressCancel = _ => {
             closeAction();
             Routes.(push(Operations));
           };
           <SubmittedView
             hash
             onPressCancel
             submitText=I18n.Btn.go_operations
           />;
         }}
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
