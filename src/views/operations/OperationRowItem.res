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
    style(~flexBasis=40.->dp, ~minWidth=40.->dp, ())
  }
})

module CellID = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=40.->dp, ~minWidth=40.->dp, ())
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

let paddingLeftAlignWithID = 64. // 40 + right margin
let styles = {
  open Style
  StyleSheet.create({
    "image": style(~marginLeft=10.->dp, ~width=19.->dp, ~height=19.->dp, ()),
    "pendingDetails": style(
      ~paddingHorizontal=(paddingLeftAlignWithID +. 2. +. 20.)->dp,
      ~paddingVertical=26.->dp,
      ~borderBottomLeftRadius=4.,
      ~borderBottomRightRadius=4.,
      (),
    ),
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
        | Token(_, {amount, kind, contract}) =>
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

let cellType = i18n => <CellType> {i18n->Typography.body1} </CellType>

let cellFee = (account: Alias.t, source, fee) => {
  let fee = account.address == source ? fee : Tez.zero
  <CellFee>
    {fee == Tez.zero ? React.null : I18n.tez_amount(fee->Tez.toString)->Typography.body1}
  </CellFee>
}

module RevealRow = {
  @react.component
  let make = (~account: Alias.t, ~source, ~fee) => {
    <>
      {cellType(I18n.operation_reveal)}
      <CellAmount />
      {cellFee(account, source, fee)}
      <CellAddress />
      <CellAddress />
    </>
  }
}

module OriginationRow = {
  @react.component
  let make = (~account: Alias.t, ~aliases, ~tokens, ~fee, ~source, ~origination) => {
    <>
      {cellType(I18n.operation_origination)}
      <CellAmount />
      {cellFee(account, source, fee)}
      <CellAddress />
      <CellAddress>
        {Option.mapWithDefault(origination, React.null, x =>
          getContactOrRaw(
            aliases,
            tokens,
            x.Operation.Origination.contract->PublicKeyHash.buildContract->Result.getExn,
          )
        )}
      </CellAddress>
      <View />
    </>
  }
}

module DelegationRow = {
  @react.component
  let make = (~account: Alias.t, ~aliases, ~tokens, ~fee, ~source, ~delegation) => {
    <>
      {cellType(I18n.operation_delegation)}
      <CellAmount />
      {cellFee(account, source, fee)}
      <CellAddress> {getContactOrRaw(aliases, tokens, source)} </CellAddress>
      {delegation.Operation.Delegation.delegate->Option.mapWithDefault(
        <CellAddress> {I18n.delegation_removal->Typography.body1(~numberOfLines=1)} </CellAddress>,
        d => <CellAddress> {getContactOrRaw(aliases, tokens, d)} </CellAddress>,
      )}
    </>
  }
}

module UnknownRow = {
  @react.component
  let make = () => {
    <> {cellType(I18n.unknown_operation)} <CellAmount /> <CellFee /> <CellAddress /> </>
  }
}

module TransactionRow = {
  @react.component
  let make = (~account: Alias.t, ~aliases, ~tokens, ~fee, ~source, ~transaction, ~uniqueId) => {
    switch transaction {
    | Operation.Transaction.Tez({amount, destination, entrypoint})
    | Operation.Transaction.Token({amount, destination, entrypoint}, _) =>
      let (label, amount) = switch entrypoint {
      | Some(entrypoint) if entrypoint != "default" && amount == Tez.zero => (
          "{ " ++ entrypoint ++ " }",
          <CellAmount />,
        )
      | _ => (
          I18n.operation_transaction,
          <GenericCellAmount address=account.address transaction tokens tooltipSuffix=uniqueId />,
        )
      }
      <>
        {cellType(label)}
        {amount}
        {cellFee(account, source, fee)}
        <CellAddress>
          {source
          ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
          ->Option.mapWithDefault(rawUnknownAddress(source), alias => {
            alias->Typography.body1(~numberOfLines=1)
          })}
        </CellAddress>
        <CellAddress> {getContactOrRaw(aliases, tokens, destination)} </CellAddress>
      </>
    }
  }
}

@react.component
let make = memo((
  ~account: Alias.t,
  ~config: ConfigContext.env,
  ~operation: Operation.t,
  ~currentLevel,
) => {
  let aliases = StoreContext.Aliases.useGetAll()
  let tokens = StoreContext.Tokens.useGetAll()
  let addToast = LogsContext.useToast()
  let {fee, source, payload} = operation

  <Table.Row.Bordered>
    {switch payload {
    | Reveal(_reveal) => <RevealRow account source fee />
    | Transaction(transaction) =>
      let uniqueId = operation->Operation.uniqueId->Operation.uniqueIdToString
      <TransactionRow fee source account aliases transaction tokens uniqueId />
    | Origination(origination) => <OriginationRow account source aliases tokens fee origination />
    | Delegation(delegation) => <DelegationRow account aliases tokens fee source delegation />
    | Unknown => <UnknownRow />
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

type pending_step = Simulation | Sign(Protocol.Simulation.results, Protocol.batch) | Done(string)

module Pending_SignView = {
  @react.component
  let make = (~signer, ~dryRun, ~operation, ~setStep) => {
    let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()
    let state = React.useState(() => None)
    let signOpStep = React.useState(() => SignOperationView.SummaryStep)
    let sendOperation = (~operation: Protocol.batch, signingIntent) => {
      sendOperation({operation: operation, signingIntent: signingIntent})->Promise.tapOk(({
        hash,
      }) => {
        setStep(Done(hash))
      })
    }
    let loading = operationRequest->ApiRequest.isLoading
    <SignOperationView signer state signOpStep dryRun operation sendOperation loading />
  }
}

module Pending = {
  let btnStyle = Style.array([
    FormStyles.formSubmit,
    Style.style(~marginLeft=16.->Style.dp, ~marginTop=0.->Style.dp, ()),
  ])

  module ActionButton = {
    @react.component
    let make = (
      ~style=btnStyle,
      ~entrypoint,
      ~text,
      ~multisig,
      ~signer: Account.t,
      ~id,
      ~disabled,
      ~callback=() => (),
      ~title=?,
    ) => {
      let (
        sendOperationSimulateRequest,
        sendOperationSimulate,
      ) = StoreContext.Operations.useSimulate()
      let (modalStep, setModalStep) = React.useState(() => Simulation)
      let setStep = x => {setModalStep(_ => x)}
      let (openAction, closeAction, wrapModal) = ModalAction.useModal()
      let closeAction = () => {
        closeAction()
        callback()
      }
      let loadingSign =
        sendOperationSimulateRequest->ApiRequest.isLoading &&
          {
            switch modalStep {
            | Done(_) => false
            | _ => true
            }
          }
      let onPress = _ => {
        let parameter =
          ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline(
            id->ReBigNumber.toString,
          )
          ->Result.getExn
          ->Option.getExn
        let destination = multisig.Multisig.address
        let transfer = ProtocolHelper.Multisig.makeCall(~parameter, ~entrypoint, ~destination)
        let transfers = [transfer]
        let source = signer
        let operation = ProtocolHelper.Multisig.wrap(~source, transfers)

        sendOperationSimulate(operation)->Promise.getOk(dryRun => {
          setStep(Sign(dryRun, operation))
          openAction()
        })
      }
      let title = switch modalStep {
      | Done(_) => None
      | _ => title
      }
      <>
        <Buttons.SubmitPrimary text onPress style loading=loadingSign disabled />
        {wrapModal(
          <ModalFormView ?title closing=ModalFormView.Close(_ => closeAction())>
            {switch modalStep {
            | Simulation => <LoadingView />
            | Sign(dryRun, operation) => <Pending_SignView signer dryRun operation setStep />
            | Done(hash) =>
              <SubmittedView
                hash onPressCancel={_ => closeAction()} submitText=I18n.Btn.go_operations
              />
            }}
          </ModalFormView>,
        )}
      </>
    }
  }

  module ApproveButton = {
    @react.component
    let make = (~text=I18n.Btn.sign, ~style=?, ~multisig, ~signer, ~id, ~disabled, ~callback=?) => {
      <ActionButton
        entrypoint="approve"
        ?style
        title=I18n.Title.confirm_operation_approval
        text
        multisig
        signer
        id
        disabled
        ?callback
      />
    }
  }

  module ApproveWithConfirmationButton = {
    @react.component
    let make = (
      ~multisig,
      ~title=I18n.Title.approval_threshold_reached,
      ~contentText=I18n.Expl.approval_threshold_reached,
      ~signer: Account.t,
      ~id,
    ) => {
      let (openAction, closeAction, wrapModal) = ModalAction.useModal()
      let cancelText = I18n.Btn.cancel

      let action =
        <ApproveButton
          text=I18n.Btn.sign_anyway multisig signer id disabled=false callback=closeAction
        />

      <>
        <Buttons.SubmitPrimary text=I18n.Btn.sign onPress={_ => openAction()} style=btnStyle />
        {wrapModal(<ModalDialogConfirm.Modal action title contentText cancelText closeAction />)}
      </>
    }
  }

  module ExecuteButton = {
    @react.component
    let make = (~multisig, ~signer: Account.t, ~id, ~disabled) => {
      <ActionButton
        entrypoint=Multisig.executionEntrypoint
        title=I18n.Title.confirm_operation_execution
        text=I18n.Btn.submit
        multisig
        signer
        id
        disabled
      />
    }
  }

  module AddToBatchButton = {
    @react.component
    let make = (~multisig, ~signer: Account.t, ~id, ~disabled) => {
      let {addTransfer, isSimulating} = GlobalBatchContext.useGlobalBatchContext()

      let handlePress = _ => {
        let parameter =
          ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline(
            id->ReBigNumber.toString,
          )
          ->Result.getExn
          ->Option.getExn
        let destination = multisig.Multisig.address

        let p: ProtocolOptions.parameter = {
          entrypoint: Multisig.executionEntrypoint->Some,
          value: parameter->Some,
        }

        let transferPayload: GlobalBatchTypes.transferPayload = (
          ProtocolAmount.Tez(Tez.zero),
          destination,
          p,
        )

        addTransfer(transferPayload, signer, () => ())
      }
      <Buttons.SubmitPrimary
        text=I18n_en.global_batch_add_multisig_exectute
        onPress=handlePress
        style=btnStyle
        disabled={disabled || isSimulating}
      />
    }
  }
  module ApproveWithReviewButton = {
    @react.component
    let make = (~multisig, ~signer: Account.t, ~id) => {
      let title = I18n.Title.unrecognized_operation
      let contentText = I18n.Expl.unrecognized_operation
      <ApproveWithConfirmationButton multisig signer id title contentText />
    }
  }

  module TransactionActionButtons = {
    @react.component
    let make = (~multisig, ~signer: Account.t, ~id, ~hasSigned, ~canSubmit, ~unknown=false) => {
      <>
        {hasSigned
          ? <ApproveButton multisig signer id disabled=true />
          : canSubmit
          ? <ApproveWithConfirmationButton multisig signer id />
          : unknown
          ? <ApproveWithReviewButton multisig signer id />
          : <ApproveButton multisig signer id disabled=false />}
        <ExecuteButton multisig signer id disabled={!canSubmit} />
        <AddToBatchButton multisig signer id disabled={!canSubmit} />
        //        <Buttons.SubmitPrimary
        //          text=I18n.Btn.global_batch_add_short onPress={_ => ()} style=btnStyle disabled={true}
        //        />
      </>
    }
  }

  let codeView = pending =>
    pending.Multisig.API.PendingOperation.raw
    ->Js.Json.parseExn
    ->Js.Json.stringifyWithSpace(4)
    ->(text => <CodeView text />)

  module UnknownDetails = {
    @react.component
    let make = (~children) => {
      let (openAction, closeAction, wrapModal) = ModalAction.useModal()
      <CellAction>
        <IconButton size=34. icon=Icons.MagnifierPlus.build onPress={_ => openAction()} />
        {wrapModal(
          <ModalFormView
            title=I18n.Title.unrecognized_operation closing=ModalFormView.Close(closeAction)>
            {I18n.Expl.unrecognized_operation->Typography.body1(~style=FormStyles.textContent)}
            children
          </ModalFormView>,
        )}
      </CellAction>
    }
  }

  @react.component
  let make = (~account: Alias.t, ~pending: Multisig.API.PendingOperation.t) => {
    let theme = ThemeContext.useTheme()
    let accounts = StoreContext.Accounts.useGetAll()
    let aliases = StoreContext.Aliases.useGetAll()
    let tokens = StoreContext.Tokens.useGetAll()
    let multisigFromAddress = StoreContext.Multisig.useGetFromAddress()
    let multisig = multisigFromAddress(account.Alias.address)->Option.getExn

    let tooltipSuffix = pending.id->ReBigNumber.toString
    let canSubmit =
      ReBigNumber.fromInt(Array.length(pending.approvals))->ReBigNumber.isGreaterThanOrEqualTo(
        multisig.Multisig.threshold,
      )
    let signed = pending.approvals->Array.length->Int.toString
    let threshold = multisig.Multisig.threshold->ReBigNumber.toString
    let header = (collapseButton, expanded) => {
      let rowStyle = expanded
        ? Style.style(
            ~backgroundColor=theme.colors.stateActive,
            ~borderTopLeftRadius=4.,
            ~borderTopRightRadius=4.,
            (),
          )->Some
        : None

      <Table.Row.Bordered ?rowStyle>
        <CellExpandToggle> {collapseButton} </CellExpandToggle>
        <CellID> {pending.id->ReBigNumber.toString->Typography.body1} </CellID>
        {switch pending.operations {
        | [Transaction(transaction)] => <>
            <CellType> {I18n.operation_transaction->Typography.body1} </CellType>
            <GenericCellAmount address=account.address transaction tokens tooltipSuffix />
            <CellAddress>
              {getContactOrRaw(
                aliases,
                tokens,
                Operation.Transaction.Accessor.destination(transaction),
              )}
            </CellAddress>
          </>
        | [Delegation({delegate})] => <>
            {cellType(I18n.operation_delegation)}
            <CellAmount />
            {delegate->Option.mapWithDefault(
              <CellAddress>
                {I18n.delegation_removal->Typography.body1(~numberOfLines=1)}
              </CellAddress>,
              d => <CellAddress> {getContactOrRaw(aliases, tokens, d)} </CellAddress>,
            )}
          </>
        | [Unknown] | _ => <> {cellType(I18n.unknown_operation)} <CellAmount /> <CellAddress /> </>
        }}
        <CellSignatures style={Style.style(~flexDirection=#row, ())}>
          {
            let color = canSubmit ? theme.colors.textPositive : theme.colors.textHighEmphasis
            <>
              <Icons.Key size=20. color />
              {I18n.a_of_b(signed, threshold)->Typography.body1(
                ~style=Style.style(~color, ~marginLeft=8.->Style.dp, ()),
              )}
            </>
          }
        </CellSignatures>
        {Array.length(pending.operations) > 1 || pending.operations == [Unknown]
          ? <UnknownDetails> {codeView(pending)} </UnknownDetails>
          : React.null}
      </Table.Row.Bordered>
    }
    <ContractRows.Collapsable header expanded=false>
      <View
        style={Style.array([
          styles["pendingDetails"],
          Style.style(
            ~backgroundColor=theme.colors.stateActive,
            ~borderTopColor=theme.colors.stateDisabled,
            ~borderTopWidth=1.,
            (),
          ),
        ])}>
        <View style={styles["numberOfApproval"]}>
          {
            let size = 24.
            let style = {
              open Style
              style(~marginRight=10.->dp, ())
            }
            ReBigNumber.fromInt(
              pending.approvals->Array.length,
            )->ReBigNumber.isGreaterThanOrEqualTo(multisig.Multisig.threshold)
              ? <Icons.CheckFill size style color=theme.colors.textPositive />
              : <Icons.RadioOff size style color=theme.colors.textMediumEmphasis />
          }
          {I18n.approved_a_of_b(signed, threshold)->Typography.overline2}
        </View>
        <View>
          {multisig.Multisig.signers
          ->Array.mapWithIndex((i, owner) => {
            let hasSigned = Js.Array.includes(owner, pending.approvals)
            <View key={i->Int.toString} style={styles["signerWrapper"]}>
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
                    ? styles["signerFst"]->Some
                    : Style.style(
                        ~borderTopColor=theme.colors.textDisabled,
                        ~borderTopWidth=1.,
                        (),
                      )->Some
                let lst =
                  i == Js.Array.length(multisig.Multisig.signers) - 1
                    ? styles["signerLst"]->Some
                    : None
                let style = Style.arrayOption([
                  fst,
                  lst,
                  styles["signerBox"]->Some,
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
                  {switch PublicKeyHash.Map.get(accounts, owner) {
                  | Some(signer) =>
                    <TransactionActionButtons
                      signer
                      multisig
                      id=pending.id
                      hasSigned
                      canSubmit
                      unknown={pending.operations == [Unknown]}
                    />
                  | None => React.null
                  }}
                </>
              }
            </View>
          })
          ->React.array}
        </View>
      </View>
    </ContractRows.Collapsable>
  }
}
