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
module CSVPicker = BatchView.CSVFilePicker
module CSVFormatLink = BatchView.CSVFormatLink

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~display=#flex, ~flexDirection=#row, ()),
    "leftCol": style(~flex=4., ()),
    "rightCol": style(~flex=1., ~marginLeft=12.->dp, ()),
  })
}

// Don"t want to repeat this
module Container = {
  @react.component
  let make = (~children) => <View style={styles["container"]}> children </View>
}

module LeftCol = {
  @react.component
  let make = (~children) => <View style={styles["leftCol"]}> children </View>
}

module RightCol = {
  @react.component
  let make = (~children) => <View style={styles["rightCol"]}> children </View>
}

module TransactionCounter = {
  @react.component
  let make = (~operations) => {
    let count = Array.length(operations)
    let transactionsDisplay = age => "Transactions (" ++ Int.toString(age) ++ ")" // Interpolation as described in docs not working
    let style = {
      open Style
      style(~marginTop=4.->dp, ~marginBottom=4.->dp, ~position=#absolute, ~left=0.->dp, ())
    }
    {transactionsDisplay(count)->Typography.overline2(~style)}
  }
}

module Wrapper = {
  @react.component
  let make = (~children) =>
    <View
      style={
        open Style
        style(
          ~display=#flex,
          ~flexDirection=#row,
          ~justifyContent=#flexEnd,
          ~alignItems=#center,
          (),
        )
      }>
      children
    </View>
}
let buildSummaryContent = OperationSummaryView.Batch.buildSummaryContent

let buildSummary = (~dryRun, ~operations) =>
  dryRun->Option.map(dryRun => buildSummaryContent(operations, dryRun))

let csvRowToTransferPayloads = (csvRows: CSVEncoding.t) =>
  csvRows
  ->List.mapReverse(t =>
    switch t.data {
    | Transfer.FA2Batch(_) => assert false
    | Transfer.Simple({destination, amount}) => (amount, destination, t.parameter)
    }
  )
  ->List.toArray

type indexedValidStates = array<((int, option<int>), Umami.SendForm.validState)>

@react.component
let make = () => {
  let {
    dryRun,
    removeBatchItem,
    replaceBatchItem,
    resetGlobalBatch,
    batch,
    addTransfers,
    setBatchAndSim,
  } = GlobalBatchContext.useGlobalBatchContext()

  StoreContext.SelectedAccount.useGetAtInit()->Option.mapWithDefault(React.null, account => {
    let pkh = account.Alias.address
    let onAddCSVList = (csvRows: CSVEncoding.t) =>
      addTransfers(pkh, csvRows->csvRowToTransferPayloads)
    let dryRun = dryRun(pkh)
    let operations = batch(pkh)->Option.getWithDefault([])
    let summary = account.kind == Some(Multisig) ? None : buildSummary(~dryRun, ~operations)
    <Page>
      <Typography.Headline style=Styles.title>
        {I18n.Title.global_batch->React.string}
      </Typography.Headline>
      <Container>
        <LeftCol>
          <AccountElements.Selector.Simple account />
          <Wrapper>
            <TransactionCounter operations /> <CSVPicker onAddCSVList /> <CSVFormatLink />
          </Wrapper>
          {[] == operations
            ? <BatchEmptyView />
            : {
                let removeBatchItem = removeBatchItem(pkh)
                let replaceBatchItem = (a, b) => replaceBatchItem(pkh, a, b)
                let onDeleteAll = () => resetGlobalBatch(pkh)
                <GlobalBatchDataTableWithModals
                  account
                  batch=operations
                  onDeleteAll
                  dryRun
                  removeBatchItem
                  replaceBatchItem
                  setBatchAndSim
                />
              }}
        </LeftCol>
        <RightCol>
          {
            let resetGlobalBatch = () => resetGlobalBatch(pkh)
            <>
              <SubmitGlobalBatchButton dryRun account operations resetGlobalBatch />
              {summary->Option.mapWithDefault(React.null, content =>
                <GlobalBatchSummaryColumn content />
              )}
            </>
          }
        </RightCol>
      </Container>
    </Page>
  })
}
