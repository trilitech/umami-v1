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

let sort = op => {
  open Operation
  op->SortArray.stableSortBy(({timestamp: t1}, {timestamp: t2}) =>
    -Pervasives.compare(Js.Date.getTime(t1), Js.Date.getTime(t2))
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

  module Preparation = {
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

type tab = History | Preparation

type foo = OperationRowItem.Preparation.foo

let test_data1 = {
  let source = Multisig.test_data[0]->Option.getExn
  let destination = Obj.magic("tz2M5GjuWooVsSbEmc8TWVRpHP8hxrqsoF6D")
  let payload = Operation.Transaction(
    Tez({
      Operation.Transaction.amount: Tez.fromMutezInt(6000),
      destination: destination,
      parameters: None,
    }),
  )
  let signers = [Obj.magic("tz2M5GjuWooVsSbEmc8TWVRpHP8hxrqsoF6D")]
  {
    OperationRowItem.Preparation.id: 0,
    payload: payload,
    source: source,
    signers: signers,
  }
}

let test_data2 = {
  ...test_data1,
  signers: [
    Obj.magic("tz2M5GjuWooVsSbEmc8TWVRpHP8hxrqsoF6D"),
    Obj.magic("tz2QUKUe6JSve7PgmbeJPUkWPyAtjEWPoCtc"),
    Obj.magic("tz1UMMZHpwmrQBHjAdwcL8uMe3fjZSecEz3F"),
  ],
}

let test_data = [test_data1, test_data2]

let renderOperation = (account, config, currentLevel, operation: Operation.t) => {
  let key = {
    open Operation
    operation->uniqueId->uniqueIdToString
  }
  <OperationRowItem account config key operation currentLevel />
}

let renderPreparation = (account, preparation: foo) => {
  let key = preparation.OperationRowItem.Preparation.id->Int.toString
  <OperationRowItem.Preparation account key preparation />
}

@react.component
let make = (~account: Account.t) => {
  let operationsRequest = StoreContext.Operations.useLoad(~address=account.address, ())
  let operationsReload = StoreContext.Operations.useResetAll()
  let preparationsRequest: Umami.ApiRequest.t<array<foo>> = ApiRequest.Done(
    Ok(test_data),
    ApiRequest.ValidSince(0.),
  )

  let config = ConfigContext.useContent()
  let dimensions = Dimensions.useWindowDimensions()
  let (tab, setTab) = React.useState(() => History)

  let footerStyle = {
    open Style
    array([
      style(~width=(dimensions.width -. NavBar.width -. 36. *. 2.)->dp, ~left=36.->dp, ()),
      unsafeStyle({"position": "sticky"}),
    ])
  }

  <View style={styles["container"]}>
    <OperationsHeaderView account>
      <RefreshButton onRefresh=operationsReload loading={operationsRequest->ApiRequest.isLoading} />
    </OperationsHeaderView>
    <View style={styles["tabSelector"]}>
      <SegmentedButtons
        selectedValue=tab
        setSelectedValue=setTab
        buttons=[(I18n.Btn.history, History), (I18n.Btn.in_preparation, Preparation)]
      />
    </View>
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
      {switch tab {
      | History => <>
          <OperationsTableHeaderView.History />
          {switch operationsRequest {
          | ApiRequest.Done(Ok(response), _) =>
            <Pagination
              elements={response.operations->sort}
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
      | Preparation => <>
          <OperationsTableHeaderView.Preparation />
          {switch preparationsRequest {
          | ApiRequest.Done(Ok(elements), _) =>
            <Pagination
              elements
              renderItem={renderPreparation(account)}
              emptyComponent={I18n.empty_preparations->React.string}
              footerStyle
            />
          | Done(Error(error), _) => error->Errors.toString->React.string
          | NotAsked
          | Loading(Some(_))
          | Loading(None) =>
            <LoadingView />
          }}
        </>
      }}
    </ScrollView>
  </View>
}
