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

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~flex=1., ()),
    "tabSelector": style(
      ~flexDirection=#row,
      ~marginHorizontal=LayoutConst.pagePaddingHorizontal->Style.dp,
      (),
    ),
  })
}

// FIXME: When the workaround will be removed and the Operation.t structure fixed,
// we should use the (timestamp, internal) and not (timestamp, id)
//
// Sort operations array by timestamp/id (descending)
// Using the ID fix an issue were two operations have the same timestamp
// e.g. a call to 'execute' on multisig will also produce a transfer between addresses
// but both the call operation and the (internal) trandfer operation will have the same timestamp
//
// ID is not ideal: nothing says that "a occurs before b" implies "a.op_id < b.op_id"
// but in practice it should be ok since:
// - op is likely to be an autoincrement integer
// - a will be inserted before b in database
let sort = (op: array<Operation.t>): array<Operation.t> => {
  open Operation
  op->SortArray.stableSortBy((op1, op2) =>
    switch Pervasives.compare(Js.Date.getTime(op2.timestamp), Js.Date.getTime(op1.timestamp)) {
    | 0 => Pervasives.compare(op2.op_id, op1.op_id)
    | x => x
    }
  )
}

module OperationsTableHeaderView = {
  module Base = {
    @react.component
    let make = (~children) =>
      <Table.Head
        style={
          open Style
          style(~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp, ())
        }>
        children
      </Table.Head>
  }

  module History = {
    @react.component
    let make = () =>
      <Base>
        <OperationRowItem.CellType>
          {Typography.overline3(I18n.operation_column_type)}
        </OperationRowItem.CellType>
        <OperationRowItem.CellAmount>
          {Typography.overline3(I18n.operation_column_amount)}
        </OperationRowItem.CellAmount>
        <OperationRowItem.CellFee>
          {Typography.overline3(I18n.operation_column_fee)}
        </OperationRowItem.CellFee>
        <OperationRowItem.CellAddress>
          {Typography.overline3(I18n.operation_column_sender)}
        </OperationRowItem.CellAddress>
        <OperationRowItem.CellAddress>
          {Typography.overline3(I18n.operation_column_recipient)}
        </OperationRowItem.CellAddress>
        <OperationRowItem.CellDate>
          {Typography.overline3(I18n.operation_column_timestamp)}
        </OperationRowItem.CellDate>
        <OperationRowItem.CellStatus>
          {Typography.overline3(I18n.operation_column_status)}
        </OperationRowItem.CellStatus>
        <OperationRowItem.CellAction />
      </Base>
  }

  module Pending = {
    @react.component
    let make = () =>
      <Base>
        <OperationRowItem.CellExpandToggle> {React.null} </OperationRowItem.CellExpandToggle> // Spacing for the expand button
        <OperationRowItem.CellID>
          {Typography.overline3(I18n.operation_column_ID)}
        </OperationRowItem.CellID>
        <OperationRowItem.CellType>
          {Typography.overline3(I18n.operation_column_type)}
        </OperationRowItem.CellType>
        <OperationRowItem.CellAmount>
          {Typography.overline3(I18n.operation_column_amount)}
        </OperationRowItem.CellAmount>
        <OperationRowItem.CellAddress>
          {Typography.overline3(I18n.operation_column_recipient)}
        </OperationRowItem.CellAddress>
        <OperationRowItem.CellSignatures>
          {Typography.overline3(I18n.operation_column_signatures)}
        </OperationRowItem.CellSignatures>
      </Base>
  }
}

type tab = History | Pending

let renderOperation = (account, config, currentLevel, operation: Operation.t) => {
  let key = {
    open Operation
    operation->uniqueId->uniqueIdToString
  }
  <OperationRowItem account config key operation currentLevel />
}

let renderPending = (expanded, multisig, pending: Multisig.API.PendingOperation.t) => {
  let key = pending.Multisig.API.PendingOperation.id->ReBigNumber.toString
  <OperationRowItem.Pending expanded multisig key pending />
}

let footerStyle = width => {
  open Style
  array([
    style(~left=36.->dp, ~width=(width -. NavBar.width -. 36. *. 2.)->dp, ()),
    unsafeStyle({"position": "sticky"}),
  ])
}

module Base = {
  @react.component
  let make = (~account: Alias.t, ~onRefresh, ~loading, ~headerExt=React.null, ~children) => {
    <View style={styles["container"]}>
      <OperationsHeaderView account> <RefreshButton onRefresh loading /> </OperationsHeaderView>
      {headerExt}
      /* Vertical ScrollView can't scroll horizontally,
       so we Typography.overline3 it in an horizontal ScrollView.
       Items in a row have a total of 1272px width
       (count flex-basis and spacing of OperationsTableHeaderView)
       You need to set this value or container will only use width
       of the window (or something like this) */
      <ScrollView
        horizontal=true
        contentContainerStyle={
          open Style
          style(~flexDirection=#column, ~flexBasis=1272.->dp, ~flexGrow=1., ())
        }>
        children
      </ScrollView>
    </View>
  }
}

module OperationsHistory = {
  @react.component
  let make = (~account: Alias.t, ~request) => {
    let config = ConfigContext.useContent()
    let dimensions = Dimensions.useWindowDimensions()
    let footerStyle = footerStyle(dimensions.width)
    <>
      <OperationsTableHeaderView.History />
      {switch request {
      | ApiRequest.Done(Ok(response), _) =>
        <Pagination
          elements={response.OperationApiRequest.operations->sort}
          renderItem={renderOperation(account, config, response.currentLevel)}
          emptyComponent={I18n.empty_operations->React.string}
          footerStyle
        />
      | Done(Error(error), _) => error->Errors.toString->React.string
      | NotAsked
      | Loading(Some(_))
      | Loading(None) =>
        <LoadingView />
      }}
    </>
  }
}

module OperationsPending = {
  @react.component
  let make = (~account: Alias.t, ~expanded, ~request) => {
    let dimensions = Dimensions.useWindowDimensions()
    let footerStyle = footerStyle(dimensions.width)

    let multisigFromAddress = StoreContext.Multisig.useGetFromAddress()
    let multisig = multisigFromAddress(account.Alias.address)->Option.getExn
    <>
      <OperationsTableHeaderView.Pending />
      {switch request {
      | ApiRequest.Done(Ok(elements), _) =>
        let emptyComponent =
          <View
            style={Style.style(~paddingLeft=OperationRowItem.paddingLeftAlignWithID->Style.dp, ())}>
            {I18n.empty_pending_operations->React.string}
          </View>
        <Pagination
          elements={ReBigNumber.Map.valuesToArray(elements)}
          renderItem={renderPending(expanded, multisig)}
          emptyComponent
          footerStyle
        />
      | Done(Error(error), _) => error->Errors.toString->React.string
      | NotAsked
      | Loading(Some(_))
      | Loading(None) =>
        <LoadingView />
      }}
    </>
  }
}

module Multisig = {
  @react.component
  let make = (~account: Alias.t) => {
    let operationsRequest = StoreContext.Operations.useLoad(~address=account.address, ())
    let operationsReload = StoreContext.Operations.useResetAll()
    let pendingOperationsReload = StoreContext.Multisig.PendingOperations.useResetAll()
    let onRefresh = () => {
      operationsReload()
      pendingOperationsReload()
    }

    let pendingOperationsRequest = StoreContext.Multisig.PendingOperations.usePendingOperations(
      ~address=account.Alias.address,
    )
    let (tab, setTab) = React.useState(() => History)
    let expanded = React.useRef(Set.String.empty) // Used to keep pending operations open upon refresh
    let headerExt = {
      <View style={styles["tabSelector"]}>
        <SegmentedButtons
          selectedValue=tab
          setSelectedValue=setTab
          buttons=[(I18n.Btn.history, History), (I18n.Btn.pending_operations, Pending)]
        />
      </View>
    }
    <Base headerExt account onRefresh loading={operationsRequest->ApiRequest.isLoading}>
      {switch tab {
      | History => <OperationsHistory account request=operationsRequest />
      | Pending => <OperationsPending expanded account request=pendingOperationsRequest />
      }}
    </Base>
  }
}

module Implicit = {
  @react.component
  let make = (~account: Alias.t) => {
    let operationsRequest = StoreContext.Operations.useLoad(~address=account.address, ())
    let onRefresh = StoreContext.Operations.useResetAll()
    <Base account onRefresh loading={operationsRequest->ApiRequest.isLoading}>
      <OperationsHistory account request=operationsRequest />
    </Base>
  }
}

@react.component
let make = (~account: Alias.t) => {
  account.kind == Some(Alias.Multisig) ? <Multisig account /> : <Implicit account />
}
