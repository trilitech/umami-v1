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
            title=I18n.title#interaction
          />
          <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##label>
            I18n.label#parameters->React.string
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
        ~content:
           list((string, Belt.List.t(TezosClient.Transfer.Currency.t))),
        ~button=?,
      ) => {
    let content: list((string, Belt.List.t(string))) =
      content->List.map(((field, amounts)) =>
        (field, amounts->List.map(Transfer.Currency.showAmount))
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
  open UmamiCommon;

  let transactionParameters = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(entrypoint), Some(parameter)) =>
      Some(
        ProtocolOptions.TransactionParameters.{entrypoint, value: parameter},
      )
    | _ => None
    };

  let sourceDestination = (transfer: Transfer.t) => {
    let recipientLbl = I18n.title#recipient_account;
    let sourceLbl = I18n.title#sender_account;

    switch (transfer) {
    | {source, transfers: [t]} => (
        (source, sourceLbl),
        `One((
          Some(t.destination),
          recipientLbl,
          transactionParameters(
            ~entrypoint=t.tx_options.entrypoint,
            ~parameter=t.tx_options.parameter,
          ),
        )),
      )
    | {source, transfers} =>
      let destinations =
        transfers->List.map(t =>
          (
            t.destination,
            t.amount,
            transactionParameters(
              ~entrypoint=t.tx_options.entrypoint,
              ~parameter=t.tx_options.parameter,
            ),
            ProtocolOptions.optionsSet(t.tx_options),
          )
        );
      ((source, sourceLbl), `Many(destinations));
    };
  };

  let buildSummaryContent =
      (transaction: Transfer.t, dryRun: Protocol.Simulation.results) => {
    let feeSum = dryRun.simulations->Protocol.Simulation.sumFees;

    let partialFee = (I18n.label#fee, [Transfer.Currency.Tez(feeSum)]);

    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.label#implicit_reveal_fee, [Transfer.Currency.Tez(fee)])
        );

    let totals =
      transaction.transfers
      ->List.map(t => t.amount)
      ->Transfer.Currency.reduceAmounts;

    let subtotals = (I18n.label#summary_subtotal, totals);

    let totalTez = {
      let (sub, noTokens) =
        switch (totals) {
        | [Tez(a), ...t] => (a, t == [])
        | t => (Tez.zero, t == [])
        };

      (
        noTokens ? I18n.label#summary_total : I18n.label#summary_total_tez,
        [
          Transfer.Currency.Tez(
            Tez.Infix.(sub + dryRun->Protocol.Simulation.getTotalFees),
          ),
        ],
      );
    };

    Lib.List.(
      [totalTez]->addOpt(revealFee)->add(partialFee)->add(subtotals)
    );
  };

  [@react.component]
  let make =
      (
        ~style=?,
        ~transfer: Transfer.t,
        ~dryRun: Protocol.Simulation.results,
        ~editAdvancedOptions,
        ~advancedOptionsDisabled,
      ) => {
    let (source: (Account.t, string), destinations) =
      sourceDestination(transfer);
    let content = buildSummaryContent(transfer, dryRun);

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
          (I18n.label#implicit_reveal_fee, [Transfer.Currency.Tez(fee)])
        );

    let fee = (
      I18n.label#fee,
      [
        Transfer.Currency.Tez(dryRun.simulations->Protocol.Simulation.sumFees),
      ],
    );

    let total = (
      I18n.label#summary_total,
      [Transfer.Currency.Tez(dryRun->Protocol.Simulation.getTotalFees)],
    );

    [fee, ...revealFee->Option.mapWithDefault([total], r => [r, total])];
  };

  [@react.component]
  let make =
      (
        ~style=?,
        ~delegation: Protocol.delegation,
        ~dryRun: Protocol.Simulation.results,
      ) => {
    let (target, title) =
      switch (delegation.delegate) {
      | None => (None, I18n.title#withdraw_baker)
      | Some(d) => (Some(d), I18n.title#baker_account)
      };

    <Base
      ?style
      source=(delegation.source, I18n.title#delegated_account)
      destinations={`One((target, title, None))}
      content={buildSummaryContent(dryRun)}
    />;
  };
};
