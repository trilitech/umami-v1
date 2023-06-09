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

open GlobalBatchTypes
type fa2Pos =
  | First
  | Last
  | Single
  | Between

let makeGenericHeaderCell = (children, width, ~style=?, ~last=false, ()) => {
  let base = {
    open Style
    style(~flexBasis=width->dp, ~flex=last ? 1.0 : 0., ())
  }
  let style = Style.arrayOption([Some(base), style])
  <Table.Cell style> children </Table.Cell>
}
let makeHeaderCell = (title, width, ~onPress=() => (), ()) =>
  makeGenericHeaderCell(
    <Typography.Overline3 onPress={_ => onPress()}> {title->React.string} </Typography.Overline3>,
    width,
    (),
  )

let makeGenericRowCell = (children, width, ~style=?, ~last=false, ()) => {
  let base = {
    open Style
    style(~flexBasis=width->dp, ~flex=last ? 1.0 : 0., ())
  }
  <Table.Cell style={Style.arrayOption([Some(base), style])}>
    <Typography.Body1> children </Typography.Body1>
  </Table.Cell>
}

let makeRowCell = (title, width, ~last=false, ()) =>
  makeGenericRowCell(<Typography.Body1> {title->React.string} </Typography.Body1>, width, ~last, ())

let makeFeeCell = (~shouldDisplayFee, ~fee, ~style as styleArg, ~makeIcon, ~onAdvanced) => {
  open Style
  makeGenericRowCell(
    <Typography.Body1
      style={style(~display=#flex, ~alignItems=#center, ~justifyContent=#spaceBetween, ())}>
      {(shouldDisplayFee ? fee : "")->React.string}
      {makeIcon->Option.mapWithDefault(React.null, makeIcon =>
        <ThemedPressable onPress={_ => onAdvanced()} style={style(~borderRadius=40., ())}>
          {makeIcon(16.)}
        </ThemedPressable>
      )}
    </Typography.Body1>,
    150.,
    ~style=styleArg,
    (),
  )
}

module Header = {
  @react.component
  let make = (~style as styleFromProp=?, ~fee, ~onDeleteAll=() => ()) => {
    open ReactNative.Style
    <Table.Head style=?styleFromProp>
      {makeGenericHeaderCell(React.null, 10., ~style=style(~minWidth=30.->dp, ()), ())}
      {makeHeaderCell(I18n.global_batch_column_type, 130., ())}
      {makeHeaderCell(I18n.global_batch_subject, 130., ())}
      {makeHeaderCell(I18n.global_batch_recipient, 130., ())}
      {fee ? makeHeaderCell(I18n.global_batch_fee, 150., ()) : React.null}
      {makeGenericHeaderCell(
        <View
          style={style(
            ~display=#flex,
            ~alignItems=#center,
            ~flexDirection=#row,
            ~justifyContent=#flexEnd,
            ~paddingRight=9.->dp,
            (),
          )}>
          <DeleteGlobalBatchButton onPress={_ => onDeleteAll()} />
        </View>,
        10.,
        ~style=style(~flex=1., ~marginRight=0.->dp, ()),
        (),
      )}
    </Table.Head>
  }
}

let getContactOrRaw = OperationUtils.getContactOrRaw

let useGetContactOrRaw = () => {
  let aliases = StoreContext.Aliases.useGetAll()
  let tokens = StoreContext.Tokens.useGetAll()
  ()
  address => getContactOrRaw(aliases, tokens, address)
}

let aliasToAdress = (r: FormUtils.Alias.t) =>
  switch r {
  | Address(pkHash) => Some(pkHash)
  | Alias(_) => None
  }

module TransferRowDisplay = {
  @react.component
  let make = (
    ~amount,
    ~recipient,
    ~fee,
    ~parameter: ProtocolOptions.parameter,
    ~onDelete,
    ~onEdit,
    ~onAdvanced,
    ~fa2Position,
    ~onDetails,
  ) => {
    open Protocol.Amount
    open ReactNative.Style
    let theme = ThemeContext.useTheme()
    let getContactOrRaw = useGetContactOrRaw()

    let isContractCall = ProtocolHelper.Transfer.isNonNativeContractCall(recipient, amount)

    // Disabled edit on all contact calls on just on multisig ?
    let isExecuteMultisig = parameter.entrypoint == Multisig.Entrypoint.execute->Some

    let matchPos = wanted =>
      switch fa2Position {
      | Some(actual) => actual == wanted
      | None => false
      }

    let isFirst = matchPos(First)
    let isLast = matchPos(Last)
    let isSingle = matchPos(Single)
    // Not good to check this like this
    let isFa2 = switch fa2Position {
    | Some(_) => true
    | None => false
    }

    let isNFT = switch amount {
    | Token({token}) => TokenRepr.isNFT(token)
    | Tez(_) => false
    }

    let ty = isContractCall ? I18n.operation_contract_call : I18n.operation_transaction

    let amount = isContractCall
      ? parameter.entrypoint->ProtocolOptions.TransactionParameters.getEntrypoint
      : ProtocolAmount.show(amount)

    let shouldDisplayFee = !isFa2 || (isFa2 && (isSingle || isFirst))

    let renderContact = alias =>
      alias->aliasToAdress->Option.mapWithDefault(React.null, address => getContactOrRaw(address))

    let height = if isFirst || isLast {
      "calc(100% - 5px)"
    } else if isSingle {
      "calc(100% - 10px)"
    } else {
      "100%"
    }->StyleUtils.stringToSize

    let bottomRounded = isSingle || isLast
    let topRounded = isSingle || isFirst

    let fa2Style = style(
      ~backgroundColor=isFa2 ? theme.colors.stateDisabled : "initial",
      ~borderTopRightRadius=topRounded ? 4. : 0.,
      ~borderTopLeftRadius=topRounded ? 4. : 0.,
      ~borderBottomRightRadius=bottomRounded ? 4. : 0.,
      ~borderBottomLeftRadius=bottomRounded ? 4. : 0.,
      ~marginLeft=-10.->dp,
      ~paddingLeft=10.->dp,
      ~height,
      ~marginTop=isFirst ? 5.->dp : 0.->dp,
      ~marginBottom=isLast ? 5.->dp : 0.->dp,
      ~justifyContent=#center,
      (),
    )

    let makeIcon = (builder: Umami.Icons.builder, size) =>
      builder(~color=theme.colors.borderMediumEmphasis, ~size, ~style=style(~margin=10.->dp, ()))

    let nftIcon = makeIcon(Icons.Nft.build, 20.)
    let transferIcon = makeIcon(Icons.Send.build, 20.)

    let icon = isNFT ? nftIcon : transferIcon

    <Table.Row.Bordered>
      {makeGenericRowCell(
        <View style={style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ())}>
          {shouldDisplayFee ? icon : React.null}
        </View>,
        10.,
        ~style=style(~minWidth=30.->dp, ()),
        (),
      )}
      {makeGenericRowCell(
        <View style={style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ())}>
          {<> {(shouldDisplayFee ? ty : "")->React.string} </>}
        </View>,
        130.,
        (),
      )}
      {makeRowCell(amount, 130., ())}
      {makeGenericRowCell(renderContact(Address(recipient)), 130., ())}
      {fee->Option.mapWithDefault(React.null, fee => {
        let makeIcon = !isFa2 || isFirst || isSingle ? makeIcon(Icons.Options.build)->Some : None
        let style = fa2Style
        makeFeeCell(~shouldDisplayFee, ~fee, ~style, ~makeIcon, ~onAdvanced)
      })}
      {makeGenericRowCell(
        <View style={style(~display=#flex, ~justifyContent=#flexEnd, ~flexDirection=#row, ())}>
          <View>
            <BatchView.BuildingBatchMenu
              onDetails onEdit={isNFT || isExecuteMultisig ? None : Some(onEdit)} onDelete
            />
          </View>
        </View>,
        10.,
        ~last=true,
        (),
      )}
    </Table.Row.Bordered>
  }
}

let getFeeDisplay = (~simulation: option<Umami.Protocol.Simulation.resultElt>) =>
  switch simulation {
  | Some(value) => Tez.toString(value.fee)
  | None => ""
  }

let makeKey = ((i, j): GlobalBatchTypes.managerCoords) =>
  switch j {
  | Some(j) => Belt_Int.toString(i) ++ ("-" ++ Belt_Int.toString(j))
  | None => Belt_Int.toString(i)
  }

// Gives us the relative position of an FA2 in order to style accordingly
// First and last have rounded borders, etc...
let getFa2Pos = (
  coords: GlobalBatchTypes.managerCoords,
  allCoords: array<GlobalBatchTypes.managerCoords>,
) => {
  let (i, j) = coords

  switch j {
  | None => None
  | Some(j) =>
    let next = allCoords->Array.getBy(coords => coords == (i, Some(j + 1)))

    if j == 0 {
      switch next {
      | Some(_) => Some(First)
      | None => Some(Single)
      }
    } else {
      switch next {
      | Some(_) => Some(Between)
      | None => Some(Last)
      }
    }
  }
}

@react.component
let make = (
  ~indexedRows: array<rowData>,
  ~dryRun: option<Umami.Protocol.Simulation.results>,
  ~onDelete,
  ~onEdit,
  ~onAdvanced,
  ~onDeleteAll,
  ~onDetails,
) => {
  let allCoords = indexedRows->Array.map(((a, _)) => a)
  let elements = Array.mapWithIndex(indexedRows, (i, x) => (i, x))
  let renderItem = ((arrIndex, rowData)) => {
    let (coords, payload) = rowData
    let (managerIndex, _) = coords
    let (amount, recipient, parameter) = payload
    <TransferRowDisplay
      key={makeKey(coords)}
      amount
      recipient
      fee={Option.map(dryRun, x => getFeeDisplay(~simulation=x.simulations[managerIndex]))}
      onDelete={() => onDelete(arrIndex)}
      parameter
      onEdit={_ => onEdit(arrIndex)}
      onDetails={parameter->ProtocolHelper.Transfer.hasParams
        ? Some(_ => onDetails(arrIndex))
        : None}
      onAdvanced={_ => onAdvanced(arrIndex)}
      fa2Position={getFa2Pos(coords, allCoords)}
    />
  }
  let gutters = n => Style.style(~marginHorizontal=(LayoutConst.pagePaddingHorizontal *. n)->Style.dp, ())
  <View style={gutters(-1.)}>
    <Header style={gutters(1.)} fee={dryRun != None} onDeleteAll />
    <Pagination elements renderItem emptyComponent=React.null />
  </View>
}
