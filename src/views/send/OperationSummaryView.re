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
      "itemInfos":
        style(~padding=16.->dp, ~borderRadius=5., ~flexDirection=`row, ()),
      "accounticon": FormStyles.accountIcon,
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
  let make =
      (
        ~address: option(PublicKeyHash.t),
        ~title,
        ~default=React.null,
        ~style=?,
      ) => {
    let theme = ThemeContext.useTheme();
    let aliases = StoreContext.Aliases.useGetAll();

    let alias =
      address->Option.flatMap(alias =>
        alias->AliasHelpers.getAliasFromAddress(aliases)
      );

    <View ?style>
      <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline2>
      <View
        style=Style.(
          array([|
            styles##itemInfos,
            style(~backgroundColor=theme.colors.stateDisabled, ()),
          |])
        )>
        {alias->ReactUtils.mapOpt(alias =>
           <AliasIcon
             style=styles##accounticon
             kind={alias.Alias.kind}
             isHD=true
           />
         )}
        <View>
          {alias->ReactUtils.mapOpt(alias =>
             <Typography.Subtitle2 fontSize=16. style=styles##subtitle>
               alias.name->React.string
             </Typography.Subtitle2>
           )}
          {address->Option.mapWithDefault(default, address =>
             <Typography.Address>
               (address :> string)->React.string
             </Typography.Address>
           )}
        </View>
      </View>
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
        ~content: list((string, Belt.List.t(Transfer.Currency.t))),
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
      destinations
    </View>;
  };
};

module Transactions = {
  let transactionParameters = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(entrypoint), Some(parameter)) =>
      Some(ReTaquitoTypes.Transfer.Parameters.{entrypoint, value: parameter})
    | _ => None
    };

  let sourceDestination = (transfer: Transfer.t) => {
    let recipientLbl = I18n.Title.recipient_account;
    let sourceLbl = I18n.Title.sender_account;

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
            ProtocolOptions.txOptionsSet(t.tx_options),
          )
        );
      ((source, sourceLbl), `Many(destinations));
    };
  };

  let buildSummaryContent =
      (transaction: Transfer.t, dryRun: Protocol.Simulation.results) => {
    let feeSum = dryRun.simulations->Protocol.Simulation.sumFees;

    let partialFee = (I18n.Label.fee, [Transfer.Currency.Tez(feeSum)]);

    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.Label.implicit_reveal_fee, [Transfer.Currency.Tez(fee)])
        );

    let totals =
      transaction.transfers
      ->List.map(t => t.amount)
      ->Transfer.Currency.reduceAmounts;

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
          Transfer.Currency.Tez(
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
      | CustomAuth(_)
      | Ledger => true
      | Encrypted
      | Unencrypted => false
      };

    <Base
      ?style
      source
      destinations={Base.buildDestinations(
        smallest,
        destinations,
        Some(batchAdvancedOptions),
      )}
      content
    />;
  };
};

module Delegate = {
  let buildSummaryContent = (dryRun: Protocol.Simulation.results) => {
    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.Label.implicit_reveal_fee, [Transfer.Currency.Tez(fee)])
        );

    let fee = (
      I18n.Label.fee,
      [
        Transfer.Currency.Tez(dryRun.simulations->Protocol.Simulation.sumFees),
      ],
    );

    let total = (
      I18n.Label.summary_total,
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
      | None => (None, I18n.Title.withdraw_baker)
      | Some(d) => (Some(d), I18n.Title.baker_account)
      };

    <Base
      ?style
      source=(delegation.source, I18n.Title.delegated_account)
      destinations={Base.buildDestinations(
        false,
        `One((target, title, None)),
        None,
      )}
      content={buildSummaryContent(dryRun)}
    />;
  };
};

module Originate = {
  let buildSummaryContent = (dryRun: Protocol.Simulation.results) => {
    let revealFee =
      dryRun.revealSimulation
      ->Option.map(({fee}) =>
          (I18n.Label.implicit_reveal_fee, [Transfer.Currency.Tez(fee)])
        );

    let fee = (
      I18n.Label.fee,
      [
        Transfer.Currency.Tez(dryRun.simulations->Protocol.Simulation.sumFees),
      ],
    );

    let total = (
      I18n.Label.summary_total,
      [Transfer.Currency.Tez(dryRun->Protocol.Simulation.getTotalFees)],
    );

    [fee, ...revealFee->Option.mapWithDefault([total], r => [r, total])];
  };

  [@react.component]
  let make =
      (
        ~style=?,
        ~origination: Protocol.origination,
        ~dryRun: Protocol.Simulation.results,
      ) => {
    <Base
      ?style
      source=(origination.source, I18n.Title.contract_originator)
      destinations={
        <View style=styles##element>
          <View style=FormStyles.amountRow>
            <Typography.Overline2>
              I18n.Title.balance->React.string
            </Typography.Overline2>
            <Typography.Body1 style=styles##amount fontWeightStyle=`black>
              {I18n.tez_amount(
                 origination.balance
                 ->Option.getWithDefault(Tez.zero)
                 ->Tez.toString,
               )
               ->React.string}
            </Typography.Body1>
          </View>
          <EntityInfo
            style=styles##element
            address={origination.delegate}
            title=I18n.Title.delegate
            default={
              <Typography.Body1> I18n.none->React.string </Typography.Body1>
            }
          />
          <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##label>
            I18n.Label.storage->React.string
          </Typography.Overline2>
          <TransactionContractParams parameters={origination.storage} />
          <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##label>
            I18n.Label.code->React.string
          </Typography.Overline2>
          <TransactionContractParams parameters={origination.code} />
        </View>
      }
      content={buildSummaryContent(dryRun)}
    />;
  };
};
