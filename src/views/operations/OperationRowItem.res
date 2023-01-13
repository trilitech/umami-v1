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

module MakeCellWidth = (
  S: {
    let flexBasis: float
  },
) => Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=S.flexBasis->dp, ())
  }
})

module CellExpandToggle = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=40.->dp, ~minWidth=40.->dp, ~marginRight=0.->dp, ~alignItems=#center, ())
  }
})

module CellID = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=40.->dp, ~minWidth=40.->dp, ~alignItems=#center, ())
  }
})

module CellType = MakeCellWidth({
  let flexBasis = 90.
})

module CellAmount = MakeCellWidth({
  let flexBasis = 140.
})

module CellFee = MakeCellWidth({
  let flexBasis = 86.
})

module CellAddress = MakeCellWidth({
  let flexBasis = 180.
})

module CellStatus = MakeCellWidth({
  let flexBasis = 100.
})

module CellSignatures = MakeCellWidth({
  let flexBasis = 100.
})

module CellDate = MakeCellWidth({
  let flexBasis = 180.
})

module CellAction = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=30.->dp, ~minWidth=30.->dp, ~alignItems=#center, ())
  }
  ()
})

let styles = {
  open Style
  StyleSheet.create({
    "image": style(~marginLeft=10.->dp, ~width=19.->dp, ~height=19.->dp, ()),
  })
}

let getContactOrRaw = OperationUtils.getContactOrRaw
let rawUnknownAddress = OperationUtils.rawUnknownAddress

let status = (operation: Operation.t, currentLevel, config: ConfigContext.env) => {
  let (txt, colorStyle) = switch operation.status {
  | Mempool => (I18n.state_mempool, Some(#negative))
  | Chain =>
    let minConfirmations = config.confirmations
    let currentConfirmations = currentLevel - operation.level
    currentConfirmations > minConfirmations
      ? (I18n.state_confirmed, None)
      : (I18n.state_levels(currentConfirmations, minConfirmations), Some(#negative))
  }

  <Typography.Body1 ?colorStyle> {txt->React.string} </Typography.Body1>
}

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) => {
    let currentConfirmations = prevPros["currentLevel"] - prevPros["operation"].Operation.level

    currentConfirmations > prevPros["config"].ConfigContext.confirmations
      ? prevPros["operation"] == nextProps["operation"]
      : prevPros["operation"] == nextProps["operation"] &&
          prevPros["currentLevel"] == nextProps["currentLevel"]
  })

module AddToken = {
  @react.component
  let make = (~address, ~kind: TokenRepr.kind, ~tooltipSuffix: string, ~tokens) => {
    let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()
    let closeAction = () => closeAction()

    let apiVersion: option<Network.apiVersion> = StoreContext.useApiVersion()

    let chain =
      apiVersion->Option.map(v => v.chain)->Option.getWithDefault(Network.unsafeChainId(""))

    let tooltip = ("add_token_from_op" ++ tooltipSuffix, I18n.Tooltip.add_contract)
    let onPress = _ => openAction()

    <>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView action=#Add chain address kind tokens cacheOnlyNFT=true closeAction />
      </ModalAction>
      <IconButton icon=Icons.AddToken.build iconSizeRatio={5. /. 7.} onPress tooltip />
    </>
  }
}

module UnknownTokenAmount = {
  @react.component
  let make = (~amount, ~sign, ~address: PublicKeyHash.t, ~kind, ~tokens, ~tooltipSuffix) => {
    let tooltip = ("unknown_token" ++ tooltipSuffix, I18n.Tooltip.unregistered_token_transaction)
    <View style={OperationUtils.styles["rawAddressContainer"]}>
      <Text> {`${sign} ${amount->TokenRepr.Unit.toNatString}`->React.string} </Text>
      <IconButton
        icon=Icons.QuestionMark.build
        size=19.
        iconSizeRatio=1.
        tooltip
        disabled=true
        style={
          open Style
          style(~borderRadius=0., ~marginLeft=4.->dp, ())
        }
      />
      <AddToken address kind tooltipSuffix tokens />
    </View>
  }
}

module KnownTokenAmount = {
  @react.component
  let make = (
    ~amount,
    ~sign,
    ~token as {address, kind, symbol, decimals, _}: TokenRepr.t,
    ~registered,
    ~tokens,
    ~tooltipSuffix,
  ) =>
    <View style={OperationUtils.styles["rawAddressContainer"]}>
      <Text>
        {`${sign} ${amount->TokenRepr.Unit.toStringDecimals(decimals)} ${symbol}`->React.string}
      </Text>
      {registered ? React.null : <AddToken address kind tooltipSuffix tokens />}
    </View>
}

module NFTAmount = {
  @react.component
  let make = (~amount, ~sign, ~token: TokenRepr.t) => {
    let source = NftElements.useNftSource(token, NftFilesManager.getThumbnailURL)
    <View style={OperationUtils.styles["rawAddressContainer"]}>
      <Text>
        {`${sign} ${amount->TokenRepr.Unit.toStringDecimals(token.decimals)}`->React.string}
      </Text>
      {source->Option.mapDefault(<SVGIconNoImg />, source =>
        <Image style={styles["image"]} source />
      )}
    </View>
  }
}

module GenericCellAmount = {
  @react.component
  let make = (~address, ~transaction, ~tokens, ~tooltipSuffix) => {
    let colorStyle =
      address == transaction->Operation.Transaction.Accessor.destination ? #positive : #negative
    let sign = colorStyle == #positive ? "+" : "-"
    <CellAmount>
      <Typography.Body1 colorStyle>
        {switch transaction {
        | Operation.Transaction.Tez(transaction) =>
          I18n.tez_op_amount(sign, transaction.amount->Tez.toString)->React.string
        | Token(_, {amount, kind, contract}, _) =>
          let address = contract
          let token: option<(Token.t, bool)> = TokensLibrary.WithRegistration.getFullToken(
            tokens,
            address,
            kind->TokenRepr.kindId,
          )
          switch token {
          | None => <UnknownTokenAmount amount sign address kind tokens tooltipSuffix />
          | Some((token, _)) if token->TokenRepr.isNFT => <NFTAmount amount sign token />
          | Some((token, registered)) =>
            <KnownTokenAmount amount sign token registered tokens tooltipSuffix />
          }
        }}
      </Typography.Body1>
    </CellAmount>
  }
}

@react.component
let make = memo((
  ~account: Account.t,
  ~config: ConfigContext.env,
  ~operation: Operation.t,
  ~currentLevel,
) => {
  let aliases = StoreContext.Aliases.useGetAll()
  let tokens = StoreContext.Tokens.useGetAll()
  let addToast = LogsContext.useToast()

  <Table.Row.Bordered>
    {switch operation.payload {
    | Reveal(_reveal) => <>
        <CellType> {I18n.operation_reveal->Typography.body1} </CellType>
        <CellAmount />
        <CellFee> {I18n.tez_amount(operation.fee->Tez.toString)->Typography.body1} </CellFee>
        <CellAddress />
        <CellAddress />
      </>
    | Transaction(Token(common, _, _) as transaction)
    | Transaction(Tez(common) as transaction) =>
      let tooltipSuffix = {
        open Operation
        operation->uniqueId->uniqueIdToString
      }
      <>
        <CellType> {I18n.operation_transaction->Typography.body1} </CellType>
        <GenericCellAmount address=account.address transaction tokens tooltipSuffix />
        <CellFee> {I18n.tez_amount(operation.fee->Tez.toString)->Typography.body1} </CellFee>
        <CellAddress>
          {operation.source
          ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
          ->Option.mapWithDefault(rawUnknownAddress(operation.source), alias => {
            alias->Typography.body1(~numberOfLines=1)
          })}
        </CellAddress>
        <CellAddress> {getContactOrRaw(aliases, tokens, common.destination)} </CellAddress>
      </>
    | Origination(_origination) => <>
        <CellType> {I18n.operation_origination->Typography.body1} </CellType>
        <CellAmount />
        <CellFee />
        <CellAddress />
        <CellAddress />
        <View />
      </>
    | Delegation(delegation) => <>
        <CellType> {I18n.operation_delegation->Typography.body1} </CellType>
        <CellAmount />
        <CellFee> {I18n.tez_amount(operation.fee->Tez.toString)->Typography.body1} </CellFee>
        <CellAddress> {getContactOrRaw(aliases, tokens, operation.source)} </CellAddress>
        {delegation.delegate->Option.mapWithDefault(
          <CellAddress>
            {I18n.delegation_removal->Typography.body1(~numberOfLines=1)}
          </CellAddress>,
          d => <CellAddress> {getContactOrRaw(aliases, tokens, d)} </CellAddress>,
        )}
      </>
    | Unknown => <>
        <CellType> {I18n.unknown_operation->Typography.body1} </CellType>
        <CellAmount />
        <CellFee />
        <CellAddress />
      </>
    }}
    <CellDate> {operation.timestamp->DateFns.format("P pp")->Typography.body1} </CellDate>
    <CellStatus> {status(operation, currentLevel, config)} </CellStatus>
    <CellAction>
      <IconButton
        size=34.
        icon=Icons.OpenExternal.build
        tooltip=(
          "open_in_explorer" ++ {
            open Operation
            operation->uniqueId->uniqueIdToString
          },
          I18n.Tooltip.open_in_explorer,
        )
        onPress={_ =>
          switch Network.externalExplorer(config.network.chain) {
          | Ok(url) => System.openExternal(url ++ operation.hash)
          | Error(err) => addToast(Logs.error(~origin=Operation, err))
          }}
      />
    </CellAction>
  </Table.Row.Bordered>
})

module Preparation = {
  type foo = {
    id: int,
    payload: Operation.payload,
    source: Multisig.t,
    signers: array<PublicKeyHash.t>,
  }

  let stylesFoo = {
    open Style
    StyleSheet.create({
      "preparationDetails": style(~paddingHorizontal=58.->dp, ~paddingVertical=26.->dp, ()),
      "numberOfApproval": style(~flexDirection=#row, ~paddingVertical=2.->dp, ()),
      "signerWrapper": style(~flexDirection=#row, ~alignItems=#center, ()),
      "signerBox": style(
        ~marginLeft=30.->dp,
        ~paddingHorizontal=16.->dp,
        ~paddingVertical=12.->dp,
        ~flexDirection=#row,
        ~alignItems=#center,
        ~minHeight=68.->dp,
        ~width=416.->dp,
        (),
      ),
      "signerFst": style(~borderTopLeftRadius=4., ~borderTopRightRadius=4., ()),
      "signerLst": style(~borderBottomLeftRadius=4., ~borderBottomRightRadius=4., ()),
    })
  }

  module TransactionActionButtons = {
    @react.component
    let make = () => {
      let style = Style.array([
        FormStyles.formSubmit,
        Style.style(~marginLeft=16.->Style.dp, ~marginTop=0.->Style.dp, ()),
      ])
      <>
        <Buttons.SubmitPrimary text=I18n.Btn.sign onPress={_ => ()} style disabled={true} />
        <Buttons.SubmitPrimary text=I18n.Btn.submit onPress={_ => ()} style disabled={true} />
        <Buttons.SubmitPrimary
          text=I18n.Btn.global_batch_add_short onPress={_ => ()} style disabled={true}
        />
      </>
    }
  }

  @react.component
  let make = (~account: Account.t, ~preparation as p: foo) => {
    let theme = ThemeContext.useTheme()
    let accounts = StoreContext.Accounts.useGetAll()
    let aliases = StoreContext.Aliases.useGetAll()
    let tokens = StoreContext.Tokens.useGetAll()

    {
      switch p.payload {
      | Transaction(Token(common, _, _) as transaction)
      | Transaction(Tez(common) as transaction) =>
        let tooltipSuffix = p.id->Int.toString
        let signed = p.signers->Array.length->Int.toString
        let threshold = p.source.threshold->Int.toString
        let header = collapseButton => {
          <Table.Row.Bordered>
            <CellExpandToggle> {collapseButton} </CellExpandToggle>
            <CellID> {p.id->Int.toString->Typography.body1} </CellID>
            <CellType> {I18n.operation_transaction->Typography.body1} </CellType>
            <GenericCellAmount address=account.address transaction tokens tooltipSuffix />
            <CellAddress> {getContactOrRaw(aliases, tokens, common.destination)} </CellAddress>
            <CellSignatures> {I18n.a_of_b(signed, threshold)->Typography.body1} </CellSignatures>
          </Table.Row.Bordered>
        }
        <ContractRows.Collapsable header expanded=false>
          <View style={stylesFoo["preparationDetails"]}>
            <View style={stylesFoo["numberOfApproval"]}>
              {
                let size = 20.
                let style = {
                  open Style
                  style(~marginRight=10.->dp, ())
                }
                p.signers->Array.length >= p.source.threshold
                  ? <Icons.CheckFill size style color=theme.colors.textPositive />
                  : <Icons.RadioOff size style color=theme.colors.textMediumEmphasis />
              }
              {I18n.approved_a_of_b(signed, threshold)->Typography.overline2}
            </View>
            <View>
              {p.source.signers
              ->Array.mapWithIndex((i, owner) => {
                let hasSigned = Js.Array.includes(owner, p.signers)
                <View key={i->Int.toString} style={stylesFoo["signerWrapper"]}>
                  {
                    let textColor = hasSigned
                      ? None
                      : {
                          open Style
                          style(~color=theme.colors.textDisabled, ())->Some
                        }
                    let addressStyle = textColor
                    let iconStyle = Style.arrayOption([
                      OperationSummaryView.styles["accounticon"]->Some,
                      textColor,
                    ])
                    let nameStyle = Style.arrayOption([
                      OperationSummaryView.styles["subtitle"]->Some,
                      textColor,
                    ])
                    let fst =
                      i == 0
                        ? stylesFoo["signerFst"]->Some
                        : Style.style(
                            ~borderTopColor=theme.colors.textDisabled,
                            ~borderTopWidth=1.,
                            (),
                          )->Some
                    let lst =
                      i == Js.Array.length(p.source.signers) - 1
                        ? stylesFoo["signerLst"]->Some
                        : None
                    let style = Style.arrayOption([
                      fst,
                      lst,
                      stylesFoo["signerBox"]->Some,
                      Style.style(~backgroundColor=theme.colors.stateDisabled, ())->Some,
                    ])
                    <>
                      <View style>
                        <OperationSummaryView.EntityInfoContent
                          iconStyle nameStyle ?addressStyle address={Some(owner)}
                        />
                        {hasSigned
                          ? <Icons.CheckFill
                              style={Style.style(~marginLeft="auto"->StyleUtils.stringToSize, ())}
                              size=20.
                              color=theme.colors.textPositive
                            />
                          : React.null}
                      </View>
                      {!hasSigned && PublicKeyHash.Map.has(accounts, owner)
                        ? <TransactionActionButtons />
                        : React.null}
                    </>
                  }
                </View>
              })
              ->React.array}
            </View>
          </View>
        </ContractRows.Collapsable>
      | Reveal(_)
      | Origination(_)
      | Delegation(_)
      | Unknown => <> <CellType> {I18n.unknown_operation->Typography.body1} </CellType> </>
      }
    }
  }
}
