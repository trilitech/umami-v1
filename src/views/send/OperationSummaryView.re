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

open List.Infix;

let styles =
  Style.(
    StyleSheet.create({
      "operationSummary": style(~marginBottom=20.->dp, ~flexShrink=1., ()),
      "title": style(~marginBottom=4.->dp, ()),
      "subtitle": style(~marginBottom=4.->dp, ()),
      "label": style(~marginTop=20.->dp, ~marginBottom=4.->dp, ()),
      "iconContainer": style(~padding=25.->dp, ()),
      "amount": style(~textAlign=`right, ()),
      "element": style(~marginTop=25.->dp, ()),
    })
  );

module Content = {
  [@react.component]
  let make = (~content) => {
    <View style=styles##element>
      {{
         content
         ->List.toArray
         ->Array.map(((property, values)) =>
             <View key=property style=FormStyles.amountRow>
               <Typography.Overline2>
                 property->React.string
               </Typography.Overline2>
               <View>
                 {values
                  ->List.mapWithIndex((i, value) =>
                      <Typography.Body1
                        key={i->Int.toString}
                        style=styles##amount
                        fontWeightStyle=`black>
                        value->React.string
                      </Typography.Body1>
                    )
                  ->List.toArray
                  ->React.array}
               </View>
             </View>
           );
       }
       ->React.array}
    </View>;
  };
};

module EntityInfo = {
  [@react.component]
  let make = (~address: option(PublicKeyHash.t), ~title, ~style=?) => {
    let aliases = StoreContext.Aliases.useGetAll();

    <View ?style>
      <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline2>
      {address
       ->Option.flatMap(alias =>
           alias->AliasHelpers.getAliasFromAddress(aliases)
         )
       ->ReactUtils.mapOpt(alias =>
           <Typography.Subtitle2 fontSize=16. style=styles##subtitle>
             alias->React.string
           </Typography.Subtitle2>
         )}
      {address->ReactUtils.mapOpt(address =>
         <Typography.Address>
           (address :> string)->React.string
         </Typography.Address>
       )}
    </View>;
  };
};

module Base = {
  let buildDestinations = (smallest, destinations, button) => {
    switch (destinations) {
    | `One(address, title, parameters) =>
      switch (parameters) {
      | None => <EntityInfo style=styles##element address title />
      | Some(parameters) =>
        <>
          <EntityInfo
            style=styles##element
            address
            title=I18n.Title.interaction
          />
          <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##label>
            I18n.Label.parameters->React.string
          </Typography.Overline2>
          <TransactionContractParams parameters />
        </>
      }

    | `Many(recipients) =>
      <BatchView.Transactions smallest recipients ?button />
    };
  };

  [@react.component]
  let make =
      (
        ~style as styleProp=?,
        ~source,
        ~destinations,
        ~smallest=false,
        ~content: list((string, Belt.List.t(Transfer.Amount.t))),
        ~button=?,
      ) => {
    let content: list((string, Belt.List.t(string))) =
      content->List.map(((field, amounts)) =>
        (field, amounts->List.map(Transfer.Amount.show))
      );

    <View
      style={Style.arrayOption([|
        Some(styles##operationSummary),
        styleProp,
      |])}>
      <EntityInfo
        address={source->fst.Account.address->Some}
        title={source->snd}
      />
      {content->ReactUtils.hideNil(content => <Content content />)}
      {buildDestinations(smallest, destinations, button)}
    </View>;
  };
};

module Transactions = {
  let filterTransactions = managers =>
    managers->Array.keepMap(
      fun
      | Protocol.Transaction(t) => Some(t)
      | _ => None,
    );

  let transactionParameters = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(entrypoint), Some(parameter)) =>
      Some(ReTaquitoTypes.Transfer.Entrypoint.{entrypoint, value: parameter})
    | _ => None
    };

  let sourceDestination = (operation: Protocol.batch) => {
    let recipientLbl = I18n.Title.recipient_account;
    let sourceLbl = I18n.Title.sender_account;

    switch (operation.managers) {
    | [|Transaction({data: Simple(t), options})|] => (
        (operation.source, sourceLbl),
        `One((
          Some(t.destination),
          recipientLbl,
          transactionParameters(
            ~entrypoint=options.entrypoint,
            ~parameter=options.parameter,
          ),
        )),
      )
    | managers =>
      let managers =
        managers->Array.keepMap(
          fun
          | Transaction(t) => Some(t)
          | _ => None,
        );

      let destinations =
        managers->Array.reduce([], (acc, Transfer.{data, options}) => {
          switch (data) {
          | FA2Batch(_) => assert(false)

          | Simple(t) =>
            (
              t.destination,
              t.amount,
              transactionParameters(
                ~entrypoint=options.entrypoint,
                ~parameter=options.parameter,
              ),
              ProtocolOptions.txOptionsSet(options),
            )
            @: acc
          }
        });

      ((operation.source, sourceLbl), `Many(destinations->List.reverse));
    };
  };

  let buildSummaryContent =
      (operation: Protocol.batch, dryRun: Protocol.Simulation.results) => {
    let feeSum = dryRun.simulations->Protocol.Simulation.sumFees;

    let partialFee = (I18n.Label.fee, [Transfer.Amount.Tez(feeSum)]);

    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.Label.implicit_reveal_fee, [Transfer.Amount.Tez(fee)])
        );

    let amounts =
      operation.managers
      ->filterTransactions
      ->Transfer.reduceTransfers((acc, t) => t.Transfer.amount @: acc);

    let totals = amounts->Transfer.Amount.reduce;

    let subtotals = (I18n.Label.summary_subtotal, totals);

    let totalTez = {
      let (sub, noTokens) =
        switch (totals) {
        | [Tez(a), ...t] => (a, t == [])
        | t => (Tez.zero, t == [])
        };

      (
        noTokens ? I18n.Label.summary_total : I18n.Label.summary_total_tez,
        [
          Transfer.Amount.Tez(
            Tez.Infix.(sub + dryRun->Protocol.Simulation.getTotalFees),
          ),
        ],
      );
    };

    List.Infix.(subtotals @: partialFee @: revealFee @? [totalTez]);
  };

  [@react.component]
  let make =
      (
        ~style=?,
        ~operation: Protocol.batch,
        ~dryRun: Protocol.Simulation.results,
        ~editAdvancedOptions,
        ~advancedOptionsDisabled,
      ) => {
    let (source: (Account.t, string), destinations) =
      sourceDestination(operation);
    let content = buildSummaryContent(operation, dryRun);

    let theme = ThemeContext.useTheme();

    let batchAdvancedOptions = (i, optionsSet) => {
      let color =
        advancedOptionsDisabled
          ? theme.colors.iconDisabled
          : optionsSet
              ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;

      <IconButton
        disabled=advancedOptionsDisabled
        size=40.
        icon={(~color as _=?) => Icons.Options.build(~color)}
        onPress={_ => editAdvancedOptions(i)}
      />;
    };

    let smallest =
      switch (source->fst.kind) {
      | Ledger => true
      | Encrypted
      | Unencrypted => false
      };

    <Base
      ?style
      smallest
      source
      destinations
      content
      button=batchAdvancedOptions
    />;
  };
};

module Delegate = {
  let buildSummaryContent = (dryRun: Protocol.Simulation.results) => {
    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.Label.implicit_reveal_fee, [Transfer.Amount.Tez(fee)])
        );

    let fee = (
      I18n.Label.fee,
      [Transfer.Amount.Tez(dryRun.simulations->Protocol.Simulation.sumFees)],
    );

    let total = (
      I18n.Label.summary_total,
      [Transfer.Amount.Tez(dryRun->Protocol.Simulation.getTotalFees)],
    );

    [fee, ...revealFee->Option.mapWithDefault([total], r => [r, total])];
  };

  [@react.component]
  let make =
      (
        ~style=?,
        ~delegation: Protocol.delegation,
        ~source,
        ~dryRun: Protocol.Simulation.results,
      ) => {
    let (target, title) =
      switch (delegation.delegate) {
      | None => (None, I18n.Title.withdraw_baker)
      | Some(d) => (Some(d), I18n.Title.baker_account)
      };

    <Base
      ?style
      source=(source, I18n.Title.delegated_account)
      destinations={`One((target, title, None))}
      content={buildSummaryContent(dryRun)}
    />;
  };
};
