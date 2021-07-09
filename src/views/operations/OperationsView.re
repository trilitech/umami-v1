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
      "container": style(~flex=1., ()),
      "list": style(~flex=1., ()),
      "listContent":
        style(
          ~flex=1.,
          ~paddingTop=4.->dp,
          ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

let renderItem =
    (
      currentLevel,
      renderItemProps: VirtualizedList.renderItemProps(Operation.Read.t),
    ) => {
  let operation = renderItemProps.item;
  <OperationRowItem operation currentLevel />;
};

let keyExtractor = (operation: Operation.Read.t, _i) => {
  operation.hash ++ Int.toString(operation.op_id);
};

let _ListEmptyComponent = () =>
  <Table.Empty> I18n.t#empty_operations->React.string </Table.Empty>;

let sort = op =>
  Operation.Read.(
    op->SortArray.stableSortBy(({timestamp: t1}, {timestamp: t2}) =>
      - Pervasives.compare(Js.Date.getTime(t1), Js.Date.getTime(t2))
    )
  );

module Component = {
  [@react.component]
  let make = (~account: Account.t) => {
    let operationsRequest =
      StoreContext.Operations.useLoad(~address=account.address, ());

    let operationsReload = StoreContext.Operations.useResetAll();

    <View style=styles##container>
      <OperationsHeaderView>
        <RefreshButton
          onRefresh=operationsReload
          loading={operationsRequest->ApiRequest.isLoading}
        />
      </OperationsHeaderView>
      {ApiRequest.(
         switch (operationsRequest) {
         | Done(Ok(operations), _)
         | Loading(Some(operations)) =>
           <DocumentContext.FlatList
             style=styles##list
             contentContainerStyle=styles##listContent
             data={operations->fst->sort}
             initialNumToRender=20
             keyExtractor
             renderItem={renderItem(operations->snd)}
             _ListEmptyComponent
           />
         | Done(Error(error), _) => error->React.string
         | NotAsked
         | Loading(None) => <LoadingView />
         }
       )}
    </View>;
  };
};

module Placeholder = {
  [@react.component]
  let make = () => {
    <View style=styles##container>
      <OperationsHeaderView>
        <RefreshButton onRefresh={() => ()} loading=true />
      </OperationsHeaderView>
      <LoadingView />
    </View>;
  };
};

[@react.component]
let make = () => {
  let account = StoreContext.SelectedAccount.useGet();

  switch (account) {
  | Some(account) => <Component account />
  | None => <Placeholder />
  };
};
