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

open Protocol;
module CSVPicker = BatchView.Transactions.CSVFilePicker;
module CSVFormatLink = BatchView.Transactions.CSVFormatLink;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~display=`flex, ~flexDirection=`row, ~marginTop="6px", ()),
      "leftCol": style(~flex=4., ()),
      "rightCol": style(~flex=1., ~marginLeft="12px", ()),
    })
  );

// Don"t want to repeat this
module Container = {
  [@react.component]
  let make = (~children) => <View style=styles##container> children </View>;
};

module LeftCol = {
  [@react.component]
  let make = (~children) => <View style=styles##leftCol> children </View>;
};

module RightCol = {
  [@react.component]
  let make = (~children) => <View style=styles##rightCol> children </View>;
};

module BatchHeader = {
  [@react.component]
  let make = () => {
    <Page.Header>
      <Typography.Body1>
        {React.string(I18n.Expl.global_batch_header)}
      </Typography.Body1>
    </Page.Header>;
  };
};

module TransactionCounter = {
  [@react.component]
  let make = (~batch) => {
    let getTransactionCount = (b: option(Protocol.batch)) => {
      switch (b) {
      | Some(b) => b.managers->Array.length
      | None => 0
      };
    };
    let count = getTransactionCount(batch);
    let transactionsDisplay = age =>
      "Transactions (" ++ Int.toString(age) ++ ")"; // Interpolation as described in docs not working

    <Typography.Overline2
      style=Style.(
        style(
          ~marginTop="4px",
          ~marginBottom="4px",
          ~position=`absolute,
          ~left="0px",
          (),
        )
      )>
      {React.string(transactionsDisplay(count))}
    </Typography.Overline2>;
  };
};

module Wrapper = {
  [@react.component]
  let make = (~children) => {
    <View
      style=Style.(
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~justifyContent=`flexEnd,
          ~alignItems=`center,
          (),
        )
      )>
      children
    </View>;
  };
};
let buildSummaryContent = OperationSummaryView.Batch.buildSummaryContent;

let buildSummary = (~dryRunOpt, ~operationOpt) => {
  dryRunOpt->Option.flatMap(dryRun =>
    operationOpt->Option.map(transfer =>
      buildSummaryContent(transfer, dryRun)
    )
  );
};

let csvRowToTransferPayloads = (csvRows: CSVEncoding.t) => {
  csvRows
  ->List.mapReverse(t => {
      switch (t.data) {
      | Transfer.FA2Batch(_) => assert(false)
      | Transfer.Simple({destination, amount}) => (amount, destination)
      }
    })
  ->List.toArray;
};

type indexedValidStates =
  array(((int, option(int)), Umami.SendForm.validState));

let indexToCoords = (i, vs: indexedValidStates) => {
  vs->Array.get(i)->Option.map(((coords, _)) => coords);
};

[@react.component]
let make = () => {
  let {
    dryRun,
    removeBatchItem,
    replaceBatchItem,
    resetGlobalBatch,
    batch,
    addTransfers,
    setBatchAndSim,
  } =
    GlobalBatchContext.useGlobalBatchContext();

  let selectedAccountOpt = StoreContext.SelectedAccount.useGetAtInit();

  let onAddCSVList = (csvRows: CSVEncoding.t) => {
    csvRows->csvRowToTransferPayloads->addTransfers;
  };

  let summary = buildSummary(~dryRunOpt=dryRun, ~operationOpt=batch);

  selectedAccountOpt->Option.mapWithDefault(React.null, account => {
    <Page>
      <Typography.Headline style=Styles.title>
        I18n.Title.global_batch->React.string
      </Typography.Headline>
      <Container>
        <LeftCol>
          <BatchHeader />
          <Page.Header>
            <AccountElements.Selector.Simple account />
          </Page.Header> // Page.Header needed for consistent Account display width
          <Wrapper>
            <TransactionCounter batch />
            <CSVPicker onAddCSVList />
            <CSVFormatLink />
          </Wrapper>
          {batch->Option.mapWithDefault(<BatchEmptyView />, batch => {
             dryRun->Option.mapWithDefault(React.null, dryRun =>
               <GlobalBatchDataTableWithModals
                 batch
                 onDeleteAll=resetGlobalBatch
                 dryRun
                 removeBatchItem
                 replaceBatchItem
                 simulations={dryRun.simulations}
                 setBatchAndSim
               />
             )
           })}
        </LeftCol>
        <RightCol>
          <SubmitGlobalBatchButton dryRun operation=batch resetGlobalBatch />
          {summary->Option.mapWithDefault(React.null, summary =>
             <GlobalBatchSummaryColumn content=summary />
           )}
        </RightCol>
      </Container>
    </Page>
  });
};
