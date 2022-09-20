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
open Protocol

open List.Infix

let styles = {
  open Style
  StyleSheet.create({
    "operationSummary": style(~marginBottom=20.->dp, ~flexShrink=1., ()),
    "title": style(~marginBottom=4.->dp, ()),
    "subtitle": style(~marginBottom=4.->dp, ()),
    "label": style(~marginTop=20.->dp, ~marginBottom=4.->dp, ()),
    "iconContainer": style(~padding=25.->dp, ()),
    "amount": style(~textAlign=#right, ()),
    "element": style(~marginTop=25.->dp, ()),
    "itemInfos": style(~padding=16.->dp, ~borderRadius=5., ~flexDirection=#row, ()),
    "accounticon": FormStyles.accountIcon,
  })
}

module Content = {
  @react.component
  let make = (~content) =>
    <View style={styles["element"]}>
      {content
      ->List.toArray
      ->Array.map(((property, values)) =>
        <View key=property style=FormStyles.amountRow>
          <Typography.Overline2> {property->React.string} </Typography.Overline2>
          <View>
            {values
            ->List.mapWithIndex((i, value) =>
              <Typography.Body1
                key={i->Int.toString} style={styles["amount"]} fontWeightStyle=#black>
                {value->React.string}
              </Typography.Body1>
            )
            ->List.toArray
            ->React.array}
          </View>
        </View>
      )
      ->React.array}
    </View>
}

module EntityInfo = {
  @react.component
  let make = (~address: option<PublicKeyHash.t>, ~title, ~default=React.null, ~style=?) => {
    let theme = ThemeContext.useTheme()
    let aliases = StoreContext.Aliases.useGetAll()

    let alias = address->Option.flatMap(alias => alias->AliasHelpers.getAliasFromAddress(aliases))

    <View ?style>
      <Typography.Overline2 colorStyle=#mediumEmphasis style={styles["title"]}>
        {title->React.string}
      </Typography.Overline2>
      <View
        style={
          open Style
          array([styles["itemInfos"], style(~backgroundColor=theme.colors.stateDisabled, ())])
        }>
        {alias->ReactUtils.mapOpt(alias =>
          <AliasIcon style={styles["accounticon"]} kind=alias.Alias.kind isHD=true />
        )}
        <View>
          {alias->ReactUtils.mapOpt(alias =>
            <Typography.Subtitle2 fontSize=16. style={styles["subtitle"]}>
              {alias.name->React.string}
            </Typography.Subtitle2>
          )}
          {address->Option.mapWithDefault(default, address =>
            <Typography.Address> {(address :> string)->React.string} </Typography.Address>
          )}
        </View>
      </View>
    </View>
  }
}

module Operations = {
  let styles = {
    open Style
    StyleSheet.create({
      "row": style(~flex=1., ~paddingVertical=12.->dp, ~flexDirection=#row, ()),
      "parameters": style(~marginTop=8.->dp, ()),
      "parametersContainer": style(~flex=1., ()),
      "account": style(~flex=1., ()),
      "notFirstRow": style(~borderTopWidth=1., ()),
      "amount": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
      "container": style(~marginTop=30.->dp, ()),
      "listLabelContainer": style(
        ~flexDirection=#row,
        ~alignItems=#center,
        ~justifyContent=#spaceBetween,
        (),
      ),
      "listLabel": style(~marginBottom=4.->dp, ()),
      "infoContainer": style(~flex=1., ()),
      "label": style(~marginTop=20.->dp, ~marginBottom=4.->dp, ()),
      "num": style(~width=22.->dp, ~height=44.->dp, ~marginRight=6.->dp, ~textAlign=#right, ()),
    })
  }

  let buildAmount = amount =>
    switch amount {
    | None => AccountElements.Nothing
    | Some(amount) =>
      AccountElements.Amount(
        <Typography.Body1 fontWeightStyle=#bold style={styles["amount"]}>
          {amount->React.string}
        </Typography.Body1>,
      )
    }

  module Item = {
    let itemStyles = {
      open Style
      StyleSheet.create({
        "codeLine": style(~flex=1., ~flexDirection=#row, ~marginTop=5.->dp, ()),
        "codeBlock": style(
          ~flexDirection=#row,
          ~alignItems=#center,
          ~marginRight=5.->dp,
          ~borderRadius=4.,
          ~paddingLeft=8.->dp,
          (),
        ),
      })
    }

    let code = ((code1, code2), label1, label2) => {
      let theme = ThemeContext.useTheme()

      let addToast = LogsContext.useToast()

      let code = (label, content) =>
        <View
          style={
            open Style
            array([style(~backgroundColor=theme.colors.stateDisabled, ()), itemStyles["codeBlock"]])
          }>
          <Typography.Address colorStyle=#mediumEmphasis>
            {label->React.string}
          </Typography.Address>
          <ClipboardButton copied=I18n.Log.content addToast data=content />
        </View>

      <>
        <View style={itemStyles["codeLine"]}> {code(label1, code1)} {code(label2, code2)} </View>
      </>
    }

    @react.component
    let make = (
      ~i,
      ~title,
      ~recipient,
      ~amount,
      ~origination,
      ~parameter: option<ProtocolOptions.parameter>,
      ~button=?,
    ) => {
      let aliases = StoreContext.Aliases.useGetAll()
      let theme: ThemeContext.theme = ThemeContext.useTheme()

      let michelsonToString = p =>
        p
        ->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.toString
        ->Option.getWithDefault("")

      <>
        <View
          style={
            open Style
            arrayOption([
              Some(styles["row"]),
              Some(style(~borderColor=theme.colors.borderDisabled, ())),
              Option.onlyIf(i > 0, () => styles["notFirstRow"]),
            ])
          }>
          <Typography.Subtitle1 colorStyle=#mediumEmphasis style={styles["num"]}>
            {(i + 1)->string_of_int->React.string}
          </Typography.Subtitle1>
          <View style={styles["infoContainer"]}>
            <Typography.Subtitle1 colorStyle=#mediumEmphasis>
              {title->React.string}
            </Typography.Subtitle1>
            {switch recipient {
            | Some(recipient) =>
              <AccountElements.Selector.Item
                style={styles["account"]}
                account=Alias(
                  recipient
                  ->AliasHelpers.getAliasFromAddress(aliases)
                  ->Option.getWithDefault(Alias.make(~name="", recipient)),
                )
                showAmount={buildAmount(amount)}
              />
            | None => React.null
            }}
            {parameter->ReactUtils.mapOpt(({ProtocolOptions.value: value, entrypoint}) =>
              code(
                (
                  entrypoint->ProtocolOptions.TransactionParameters.getEntrypoint,
                  value->michelsonToString,
                ),
                I18n.Label.entrypoint,
                I18n.Label.parameter,
              )
            )}
            {origination->ReactUtils.mapOpt(((impl, storage)) =>
              code(
                (impl->michelsonToString, storage->michelsonToString),
                I18n.Label.code,
                I18n.Label.storage,
              )
            )}
          </View>
          {button->ReactUtils.opt}
        </View>
      </>
    }
  }

  let listStyle = (theme: ThemeContext.theme, ledger) => {
    open Style
    style(
      ~borderColor=theme.colors.borderDisabled,
      ~minHeight=(ledger ? 150. : 300.)->dp,
      ~maxHeight=(ledger ? 200. : 400.)->dp,
      ~borderWidth=1.,
      ~borderRadius=4.,
      (),
    )
  }

  type opDescr = {
    title: string,
    address: option<PublicKeyHash.t>,
    amount: option<string>,
    originationCode: option<(ReTaquitoTypes.Code.t, ReTaquitoTypes.Storage.t)>,
    parameter: option<ProtocolOptions.parameter>,
    optionsSet: bool,
  }

  @react.component
  let make = (
    ~recipients: array<opDescr>,
    ~smallest=false,
    ~button: option<(int, 'b) => React.element>=?,
  ) => {
    let theme = ThemeContext.useTheme()

    <View style={styles["container"]}>
      <View style={styles["listLabelContainer"]}>
        <Typography.Overline2 style={styles["listLabel"]}>
          {I18n.Title.operations->React.string}
        </Typography.Overline2>
      </View>
      <DocumentContext.ScrollView style={listStyle(theme, smallest)} alwaysBounceVertical=false>
        {recipients
        ->Array.mapWithIndex((
          i,
          {title, address, amount, optionsSet, originationCode, parameter},
        ) =>
          <Item
            title
            key={string_of_int(i)}
            i
            recipient=address
            origination=originationCode
            parameter
            amount
            button=?{button->Option.map(b => b(i, optionsSet))}
          />
        )
        ->React.array}
      </DocumentContext.ScrollView>
    </View>
  }
}

module Base = {
  let buildDestinations = (smallest, destinations, button) =>
    switch destinations {
    | [{Operations.address: address, title}] =>
      <EntityInfo style={styles["element"]} address title />
    | recipients => <Operations smallest recipients ?button />
    }

  @react.component
  let make = (
    ~style as styleProp=?,
    ~source,
    ~destinations,
    ~content: list<(string, Belt.List.t<Protocol.Amount.t>)>,
  ) => {
    let content: list<(string, Belt.List.t<string>)> =
      content->List.map(((field, amounts)) => (field, amounts->List.map(Protocol.Amount.show)))

    <View style={Style.arrayOption([Some(styles["operationSummary"]), styleProp])}>
      <EntityInfo address={(source->fst).Account.address->Some} title={source->snd} />
      {content->ReactUtils.hideNil(content => <Content content />)}
      destinations
    </View>
  }
}

module Batch = {
  let filterTransfers = managers =>
    managers->Array.keepMap(x =>
      switch x {
      | Protocol.Transfer(t) => Some(t)
      | _ => None
      }
    )

  let buildSummaryContent = (operation: Protocol.batch, dryRun: Protocol.Simulation.results) => {
    let feeSum = dryRun.simulations->ProtocolHelper.Simulation.sumFees

    let partialFee = (I18n.Label.fee, list{Amount.Tez(feeSum)})

    let revealFee =
      dryRun.revealSimulation->Option.map(({fee}) => (
        I18n.Label.implicit_reveal_fee,
        list{Protocol.Amount.Tez(fee)},
      ))

    let amounts =
      operation.managers
      ->filterTransfers
      ->ProtocolHelper.Transfer.reduceArray((acc, t) => \"@:"(t.Transfer.amount, acc))

    let totals = amounts->Protocol.Amount.reduce

    let subtotals = (I18n.Label.summary_subtotal, totals)

    let totalTez = {
      let (sub, noTokens) = switch totals {
      | list{Tez(a), ...t} => (a, t == list{})
      | t => (Tez.zero, t == list{})
      }

      (
        noTokens ? I18n.Label.summary_total : I18n.Label.summary_total_tez,
        list{
          Protocol.Amount.Tez({
            open Tez.Infix
            sub + dryRun->ProtocolHelper.Simulation.getTotalFees
          }),
        },
      )
    }

    open List.Infix
    \"@:"(subtotals, \"@:"(partialFee, \"@?"(revealFee, list{totalTez})))
  }

  @react.component
  let make = (
    ~style=?,
    ~operation: Protocol.batch,
    ~dryRun: Protocol.Simulation.results,
    ~editAdvancedOptions,
    ~advancedOptionsDisabled,
    ~hideBatchDetails=false,
  ) => {
    let content = buildSummaryContent(operation, dryRun)
    open Operations

    let destinations = operation.managers->Array.map(m =>
      switch m {
      | Transfer({options, parameter, data: Simple(t: Protocol.Transfer.generic<_>)}) => {
          title: I18n.operation_transaction,
          address: Some(t.destination),
          amount: t.amount->ProtocolAmount.show->Some,
          originationCode: None,
          parameter: Some(parameter),
          optionsSet: ProtocolOptions.txOptionsSet(options),
        }
      | Transfer({options, data: FA2Batch({address})}) => {
          title: I18n.operation_token_batch,
          address: Some(address),
          amount: None,
          originationCode: None,
          parameter: None,
          optionsSet: ProtocolOptions.txOptionsSet(options),
        }
      | Delegation(delegation) =>
        let (target, title) = switch delegation.delegate {
        | Undelegate(d) => (d, I18n.Title.withdraw_baker)
        | Delegate(d) => (Some(d), I18n.Title.baker_account)
        }
        {
          address: target,
          amount: None,
          title: title,
          originationCode: None,
          parameter: None,
          optionsSet: false,
        }
      | Origination(origination) => {
          address: origination.delegate,
          amount: None,
          title: I18n.operation_origination,
          originationCode: Some((origination.code, origination.storage)),
          parameter: None,
          optionsSet: false,
        }
      }
    )

    let theme = ThemeContext.useTheme()

    let batchAdvancedOptions = (i, optionsSet) => {
      let color = advancedOptionsDisabled
        ? theme.colors.iconDisabled
        : optionsSet
        ? theme.colors.iconPrimary
        : theme.colors.iconMediumEmphasis

      <IconButton
        disabled=advancedOptionsDisabled
        size=40.
        icon={(~color as _=?) => Icons.Options.build(~color)}
        onPress={_ => editAdvancedOptions(i)}
      />
    }

    let smallest = switch operation.source.kind {
    | CustomAuth(_)
    | Ledger => true
    | Galleon
    | Encrypted
    | Unencrypted => false
    }

    <Base
      ?style
      source=(operation.source, I18n.Title.sender_account)
      destinations={hideBatchDetails
        ? React.null
        : Base.buildDestinations(smallest, destinations, Some(batchAdvancedOptions))}
      content
    />
  }
}
