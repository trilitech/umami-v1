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

module StateLenses = [%lenses
  type state = {
    fee: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
  }
];

type validState = {
  fee: option(Tez.t),
  gasLimit: option(int),
  storageLimit: option(int),
  forceLowFee: bool,
};

let extractValidState = (state: StateLenses.state): validState => {
  fee: state.fee->Tez.fromString,
  gasLimit: state.gasLimit->Int.fromString,
  storageLimit: state.storageLimit->Int.fromString,
  forceLowFee: state.forceLowFee,
};

module Form = {
  include ReForm.Make(StateLenses);

  let fromDryRun = (dr: Protocol.Simulation.results, showLimits, index) =>
    StateLenses.{
      fee:
        dr.simulations
        ->Array.get(index)
        ->Option.mapWithDefault("", sim => sim.fee->Tez.toString),
      gasLimit:
        showLimits
          ? dr.simulations
            ->Array.get(index)
            ->Option.mapWithDefault("", sim => sim.gasLimit->Int.toString)
          : "",
      storageLimit:
        showLimits
          ? dr.simulations
            ->Array.get(index)
            ->Option.mapWithDefault("", sim => sim.storageLimit->Int.toString)
          : "",

      forceLowFee: false,
    };

  let use = (showLimits, dryRun, index, onSubmit) => {
    use(
      ~schema={
        Validation.(
          Schema(
            custom(
              values => FormUtils.(emptyOr(isValidTezAmount, values.fee)),
              Fee,
            )
            + custom(
                values => FormUtils.isValidInt(values.gasLimit),
                GasLimit,
              )
            + custom(
                values => FormUtils.isValidInt(values.storageLimit),
                StorageLimit,
              ),
          )
        );
      },
      ~onSubmit=
        f => {
          onSubmit(f);
          None;
        },
      ~initialState=dryRun->fromDryRun(showLimits, index),
      ~i18n=FormUtils.i18n,
      (),
    );
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "formRowInputs":
        style(~flexDirection=`row, ~justifyContent=`center, ()),
      "formRowInputsSeparator": style(~width=13.->dp, ()),
      "formRowInput":
        style(
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=0.->dp,
          ~marginVertical=5.->dp,
          (),
        ),
    })
  );

let updateOperation = (index, values: StateLenses.state, ops: Protocol.batch) => {
  let values = values->extractValidState;
  let fallback = (o1, o2) => Option.firstSome([o1, o2]);

  let managers =
    ops.managers
    ->Array.mapWithIndex((i, op) =>
        if (index == i) {
          switch (op) {
          | Transfer(t) =>
            let options = {
              ...t.options,
              gasLimit: fallback(values.gasLimit, t.options.gasLimit),
              fee: fallback(values.fee, t.options.fee),
              storageLimit:
                fallback(values.storageLimit, t.options.storageLimit),
            };

            {...t, options}->Protocol.Transfer;

          | Delegation(d) =>
            let fee = fallback(values.fee, d.fee);
            {...d, fee}->Protocol.Delegation;

          | Origination(o) =>
            let fee = fallback(values.fee, o.fee);
            {...o, fee}->Protocol.Origination;
          };
        } else {
          op;
        }
      );

  {...ops, managers};
};

let tezDecoration = (~style) =>
  <Typography.Body1 style> I18n.tez->React.string </Typography.Body1>;

[@react.component]
let make = (~operation: Protocol.batch, ~dryRun, ~index=0, ~token, ~onSubmit) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let showLimits =
    token != None
    || operation.managers
       ->Array.get(index)
       ->Option.mapWithDefault(false, ProtocolHelper.isContractCall);

  let form =
    Form.use(
      showLimits,
      dryRun,
      index,
      ({state}) => {
        let op = updateOperation(index, state.values, operation);

        sendOperationSimulate(op)
        ->Promise.get(
            fun
            | Ok(dr) => {
                let op =
                  updateOperation(
                    index,
                    dr->Form.fromDryRun(showLimits, index),
                    op,
                  );
                onSubmit(op, dr);
              }
            | Error(_) => (),
          );
      },
    );

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <View>
    <View style=styles##formRowInputs>
      <FormGroupCurrencyInput
        label=I18n.Label.fee
        value={form.values.fee}
        handleChange={fee => form.handleChange(Fee, fee)}
        error={form.getFieldError(Field(Fee))}
        style=styles##formRowInput
        decoration=FormGroupCurrencyInput.tezDecoration
      />
      {{
         <>
           <View style=styles##formRowInputsSeparator />
           <FormGroupTextInput
             label=I18n.Label.gas_limit
             value={form.values.gasLimit}
             handleChange={form.handleChange(GasLimit)}
             error={form.getFieldError(Field(GasLimit))}
             style=styles##formRowInput
           />
         </>;
       }
       ->ReactUtils.onlyWhen(showLimits)}
      {{
         <>
           <View style=styles##formRowInputsSeparator />
           <FormGroupTextInput
             label=I18n.Label.storage_limit
             value={form.values.storageLimit}
             handleChange={form.handleChange(StorageLimit)}
             error={form.getFieldError(Field(StorageLimit))}
             style=styles##formRowInput
           />
         </>;
       }
       ->ReactUtils.onlyWhen(showLimits)}
    </View>
    <FormGroupCheckbox
      label=I18n.Label.force_low_fee
      value={form.values.forceLowFee}
      handleChange={form.handleChange(ForceLowFee)}
      error={form.getFieldError(Field(ForceLowFee))}
    />
    <Buttons.SubmitPrimary
      text=I18n.Btn.update
      loading={operationSimulateRequest->ApiRequest.isLoading}
      onPress={_ => form.submit()}
      disabledLook={!formFieldsAreValids}
    />
  </View>;
};
