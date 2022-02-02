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
      "container": style(~marginTop=30.->dp, ()),
      "totalAmount": style(~textAlign=`right, ()),
      "listLabelContainer":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "listLabel": style(~marginBottom=4.->dp, ()),
      "amount": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
      "summary": style(~marginTop=11.->dp, ()),
      "row":
        style(~flex=1., ~paddingVertical=12.->dp, ~flexDirection=`row, ()),
      "addTransaction": style(~marginBottom=10.->dp, ()),
      "notFirstRow": style(~borderTopWidth=1., ()),
      "num":
        style(
          ~width=22.->dp,
          ~height=44.->dp,
          ~marginRight=6.->dp,
          ~textAlign=`right,
          (),
        ),
      "parameters": style(~marginTop=8.->dp, ()),
      "parametersContainer": style(~flex=1., ()),
      "moreButton": style(~marginHorizontal=auto, ()),
      "account": style(~flex=1., ()),
      "csvFormat":
        style(
          ~alignItems=`flexEnd,
          ~paddingBottom=4.->dp,
          ~marginRight=10.->dp,
          (),
        ),
    })
  );

let listStyle = (theme: ThemeContext.theme, ledger) =>
  Style.(
    style(
      ~borderColor=theme.colors.borderDisabled,
      ~minHeight=(ledger ? 150. : 300.)->dp,
      ~maxHeight=(ledger ? 200. : 400.)->dp,
      ~borderWidth=1.,
      ~borderRadius=4.,
      (),
    )
  );

let buildAmount = amount => {
  AccountElements.Amount(
    <Typography.Body1 fontWeightStyle=`bold style=styles##amount>
      amount->React.string
    </Typography.Body1>,
  );
};

module Item = {
  [@react.component]
  let make = (~i, ~recipient, ~amount, ~parameters=?, ~button=?) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let theme: ThemeContext.theme = ThemeContext.useTheme();

    <View
      style=Style.(
        arrayOption([|
          Some(styles##row),
          Some(style(~borderColor=theme.colors.borderDisabled, ())),
          Option.onlyIf(i > 0, () => styles##notFirstRow),
        |])
      )>
      <Typography.Subtitle1 colorStyle=`mediumEmphasis style=styles##num>
        {(i + 1)->string_of_int->React.string}
      </Typography.Subtitle1>
      {switch (parameters) {
       | Some(parameters) =>
         <View style=styles##parametersContainer>
           <AccountElements.Selector.Item
             account={
               Alias({
                 address: recipient,
                 name: I18n.Title.interaction,
                 kind: None,
               })
             }
             showAmount=AccountElements.Nothing
           />
           <TransactionContractParams parameters style=styles##parameters />
         </View>
       | None =>
         <AccountElements.Selector.Item
           style=styles##account
           account={
             Alias({
               address: recipient,
               name:
                 recipient
                 ->AliasHelpers.getAliasFromAddress(aliases)
                 ->Option.getWithDefault(""),
               kind: None,
             })
           }
           showAmount={buildAmount(amount)}
         />
       }}
      button->ReactUtils.opt
    </View>;
  };
};

module Transactions = {
  module CSVFilePicker = {
    [@react.component]
    let make = (~onAddCSVList) => {
      let addLog = LogsContext.useAdd();
      let tokens = StoreContext.Tokens.useGetAll();

      let onChange = fileTextContent => {
        let parsedCSV = fileTextContent->CSVEncoding.parseCSV(~tokens);
        switch (parsedCSV) {
        | Result.Ok(parsedCSV) => onAddCSVList(parsedCSV)
        | Result.Error(error) =>
          addLog(true, Logs.error(~origin=Logs.Batch, error))
        };
      };

      <TextFilePicker
        text=I18n.Btn.load_file
        primary=true
        accept=".csv"
        onChange
      />;
    };
  };

  module CSVFormatLink = {
    let onPress = _ =>
      System.openExternal(
        "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/wikis/doc/Batch%20File%20Format%20Specifications",
      );

    [@react.component]
    let make = () => {
      let theme = ThemeContext.useTheme();

      <ThemedPressable.Primary
        style=Style.(
          array([|
            styles##csvFormat,
            style(~color=theme.colors.textPrimary, ()),
          |])
        )
        onPress
        accessibilityRole=`button>
        <Typography.ButtonPrimary
          style=Style.(style(~color=theme.colors.textPrimary, ()))>
          I18n.Btn.csv_format_link->React.string
        </Typography.ButtonPrimary>
      </ThemedPressable.Primary>;
    };
  };

  [@react.component]
  let make =
      (
        ~recipients:
           Belt.List.t(
             (PublicKeyHash.t, Transfer.Currency.t, option('a), 'b),
           ),
        ~smallest=false,
        ~onAddCSVList=?,
        ~button: option((int, 'b) => React.element)=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <View style=styles##container>
      <View style=styles##listLabelContainer>
        <Typography.Overline2 style=styles##listLabel>
          I18n.Label.transactions->React.string
        </Typography.Overline2>
        {onAddCSVList->Option.mapWithDefault(React.null, onAddCSVList =>
           <CSVFilePicker onAddCSVList />
         )}
      </View>
      {onAddCSVList->Option.mapWithDefault(React.null, _ => <CSVFormatLink />)}
      <DocumentContext.ScrollView
        style={listStyle(theme, smallest)} alwaysBounceVertical=false>
        {{
           recipients->List.mapWithIndex(
             (i, (recipient, amount, parameters, v)) => {
             <Item
               key={string_of_int(i)}
               i
               recipient
               amount={Transfer.Currency.showAmount(amount)}
               ?parameters
               button=?{button->Option.map(b => b(i, v))}
             />
           });
         }
         ->List.toArray
         ->React.array}
      </DocumentContext.ScrollView>
    </View>;
  };
};

module BuildingBatchMenu = {
  [@react.component]
  let make = (~i, ~onEdit, ~onDelete) => {
    let delete = _ => onDelete(i);
    let edit = _ => onEdit(i);

    <Menu
      style=styles##moreButton
      icon=Icons.More.build
      keyPopover={"batchItem" ++ i->string_of_int}>
      [|
        <Menu.Item
          key=I18n.Menu.batch_delete
          text=I18n.Menu.batch_delete
          icon=Icons.Delete.build
          onPress=delete
        />,
        <Menu.Item
          key=I18n.Menu.batch_edit
          text=I18n.Menu.batch_edit
          icon=Icons.Edit.build
          onPress=edit
        />,
      |]
    </Menu>;
  };
};

[@react.component]
let make =
    (
      ~back=?,
      ~onAddTransfer,
      ~onAddCSVList,
      ~onSubmitBatch,
      ~onDelete,
      ~onEdit,
      ~batch,
      ~loading,
    ) => {
  let theme: ThemeContext.theme = ThemeContext.useTheme();

  let recipients =
    batch->List.map((t: SendForm.validState) =>
      (t.recipient->FormUtils.Alias.address, t.amount, None, t)
    );

  let itemButton = (i, v) =>
    <BuildingBatchMenu
      i
      onDelete={_ => onDelete(i)}
      onEdit={_ => onEdit(i, v)}
    />;

  <>
    {back->ReactUtils.mapOpt(back => {
       <TouchableOpacity onPress={_ => back()} style=FormStyles.topLeftButton>
         <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
       </TouchableOpacity>
     })}
    <View style=FormStyles.header>
      <Typography.Overline1>
        I18n.Expl.batch->React.string
      </Typography.Overline1>
    </View>
    <View style={[FormStyles.amountRow, styles##summary]->ReactUtils.styles}>
      <Typography.Overline2>
        I18n.Label.summary_total->React.string
      </Typography.Overline2>
      <View>
        {batch
         ->List.map(t => t.amount)
         ->Transfer.Currency.reduceAmounts
         ->List.mapWithIndex((i, a) =>
             <Typography.Subtitle1
               style=styles##totalAmount key={i->Int.toString}>
               {a->Transfer.Currency.showAmount->React.string}
             </Typography.Subtitle1>
           )
         ->List.toArray
         ->React.array}
      </View>
    </View>
    <Transactions recipients onAddCSVList button=itemButton />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitSecondary
        style=styles##addTransaction
        text=I18n.Btn.send_another_transaction
        onPress={_ => onAddTransfer()}
        disabled=loading
      />
      <Buttons.SubmitPrimary
        text=I18n.Btn.batch_submit
        onPress={_ => onSubmitBatch(batch)}
        loading
      />
    </View>
  </>;
};
