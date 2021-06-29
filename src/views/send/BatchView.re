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
open UmamiCommon;

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
        style(~height=68.->dp, ~flexDirection=`row, ~alignItems=`center, ()),
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
      "moreButton": style(~marginHorizontal=auto, ()),
      "csvFormat":
        style(
          ~alignItems=`flexEnd,
          ~paddingBottom=4.->dp,
          ~marginRight=10.->dp,
          (),
        ),
    })
  );

let listStyle = (theme: ThemeContext.theme) =>
  Style.(
    style(
      ~borderColor=theme.colors.borderDisabled,
      ~minHeight=300.->dp,
      ~maxHeight=400.->dp,
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
  let make = (~i, ~recipient, ~amount, ~onDelete=?, ~onEdit=?) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let theme: ThemeContext.theme = ThemeContext.useTheme();
    <View
      style=Style.(
        arrayOption([|
          Some(styles##row),
          Some(style(~borderColor=theme.colors.borderDisabled, ())),
          Lib.Option.onlyIf(i > 1, () => styles##notFirstRow),
        |])
      )>
      <Typography.Subtitle1 colorStyle=`mediumEmphasis style=styles##num>
        {i->string_of_int->React.string}
      </Typography.Subtitle1>
      <AccountElements.Selector.Item
        account={
          address: recipient,
          alias:
            recipient
            ->AliasHelpers.getAliasFromAddress(aliases)
            ->Option.getWithDefault(""),
        }
        showAmount={buildAmount(amount)}
      />
      {[]
       ->Lib.List.addOpt(
           onDelete->Option.map(delete => {
             <Menu.Item
               key=I18n.menu#batch_delete
               text=I18n.menu#batch_delete
               icon=Icons.Delete.build
               onPress={_ => delete()}
             />
           }),
         )
       ->Lib.List.addOpt(
           onEdit->Option.map(edit => {
             <Menu.Item
               key=I18n.menu#batch_edit
               text=I18n.menu#batch_edit
               icon=Icons.Edit.build
               onPress={_ => edit()}
             />
           }),
         )
       ->ReactUtils.hideNil(l =>
           <Menu
             style=styles##moreButton
             icon=Icons.More.build
             keyPopover={"batchItem" ++ i->string_of_int}>
             l->List.toArray
           </Menu>
         )}
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
        let parsedCSV = fileTextContent->API.CSV.parseCSV(tokens);
        switch (parsedCSV) {
        | Result.Ok(parsedCSV) => onAddCSVList(parsedCSV)
        | Result.Error(error) =>
          addLog(true, Logs.error(error->API.handleCSVError))
        };
      };

      <TextFilePicker
        text=I18n.btn#load_file
        primary=true
        accept=".csv"
        onChange
      />;
    };
  };

  module CSVFormatLink = {
    let onPress = _ =>
      System.openExternal(
        "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/blob/master/docs/specs/batch_csv_format.md#example",
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
          I18n.btn#csv_format_link->React.string
        </Typography.ButtonPrimary>
      </ThemedPressable.Primary>;
    };
  };

  [@react.component]
  let make = (~recipients, ~showAmount, ~onAddCSVList=?, ~onDelete=?) => {
    let length = recipients->List.length;
    let theme = ThemeContext.useTheme();

    <View style=styles##container>
      <View style=styles##listLabelContainer>
        <Typography.Overline2 style=styles##listLabel>
          I18n.label#transactions->React.string
        </Typography.Overline2>
        {onAddCSVList->Option.mapWithDefault(React.null, onAddCSVList =>
           <CSVFilePicker onAddCSVList />
         )}
      </View>
      {onAddCSVList->Option.mapWithDefault(React.null, _ => <CSVFormatLink />)}
      <DocumentContext.ScrollView
        style={listStyle(theme)} alwaysBounceVertical=false>
        {{
           recipients->List.mapWithIndex((i, (onEdit, (recipient, amount))) => {
             let onDelete = onDelete->Option.map((delete, ()) => delete(i));
             <Item
               key={string_of_int(i)}
               i={length - i}
               recipient
               amount={showAmount(amount)}
               ?onDelete
               ?onEdit
             />;
           });
         }
         ->List.reverse
         ->List.toArray
         ->React.array}
      </DocumentContext.ScrollView>
    </View>;
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
      ~showAmount,
      ~reduceAmounts: _ => list(Transfer.currency),
      ~loading,
    ) => {
  let theme: ThemeContext.theme = ThemeContext.useTheme();
  let recipients =
    batch->List.mapWithIndex((i, (t: SendForm.validState, _) as v) =>
      (
        Some(() => onEdit(i, v)),
        (t.recipient->FormUtils.Account.address, t.amount),
      )
    );

  <>
    {back->ReactUtils.mapOpt(back => {
       <TouchableOpacity onPress={_ => back()} style=FormStyles.topLeftButton>
         <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
       </TouchableOpacity>
     })}
    <View style=FormStyles.header>
      <Typography.Headline>
        I18n.title#batch->React.string
      </Typography.Headline>
      <Typography.Overline1>
        I18n.expl#batch->React.string
      </Typography.Overline1>
    </View>
    <View style={[FormStyles.amountRow, styles##summary]->ReactUtils.styles}>
      <Typography.Overline2>
        I18n.label#summary_total->React.string
      </Typography.Overline2>
      <View>
        {batch
         ->List.map(((t, _)) => t.amount)
         ->reduceAmounts
         ->List.mapWithIndex((i, a) =>
             <Typography.Subtitle1
               style=styles##totalAmount key={i->Int.toString}>
               {a->showAmount->React.string}
             </Typography.Subtitle1>
           )
         ->List.toArray
         ->React.array}
      </View>
    </View>
    <Transactions recipients showAmount onAddCSVList onDelete />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitSecondary
        style=styles##addTransaction
        text=I18n.btn#send_another_transaction
        onPress={_ => onAddTransfer()}
        disabled=loading
      />
      <Buttons.SubmitPrimary
        text=I18n.btn#batch_submit
        onPress={_ => onSubmitBatch(batch)}
        loading
      />
    </View>
  </>;
};
