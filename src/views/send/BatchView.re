open ReactNative;
open UmamiCommon;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~marginTop=30.->dp, ()),
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
        style(
          ~paddingVertical=12.->dp,
          ~paddingLeft=28.->dp,
          ~paddingRight=38.->dp,
          (),
        ),
      "addTransaction": style(~marginBottom=10.->dp, ()),
      "notFirstRow": style(~borderTopWidth=1., ()),
      "num": style(~position=`absolute, ~left=10.->dp, ~top=10.->dp, ()),
      "moreButton":
        style(~position=`absolute, ~top=17.->dp, ~right=4.->dp, ()),
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
  AccountSelector.Amount(
    <Typography.Body1 fontWeightStyle=`bold style=styles##amount>
      amount->React.string
    </Typography.Body1>,
  );
};

/* let computeTotal = batch => */
/*   batch->List.reduce(0., (acc, t: SendForm.StateLenses.state) => */
/*     acc +. float_of_string(t.amount) */
/*   ); */

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
      <AccountSelector.AccountItem
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

      let onChange = fileTextContent => {
        Js.log(fileTextContent);
        let parsedCSV = fileTextContent->API.CSV.parseCSV;
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

  [@react.component]
  let make = (~recipients, ~showCurrency, ~onAddCSVList=?, ~onDelete=?) => {
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
      <DocumentContext.ScrollView
        style={listStyle(theme)} alwaysBounceVertical=false>
        {{
           recipients->List.mapWithIndex((i, (onEdit, (recipient, amount))) => {
             let onDelete = onDelete->Option.map((delete, ()) => delete(i));
             <Item
               key={string_of_int(i)}
               i={length - i}
               recipient
               amount={showCurrency(amount)}
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
      ~showCurrency,
      ~reduceAmounts,
      ~loading,
    ) => {
  let theme: ThemeContext.theme = ThemeContext.useTheme();
  let recipients =
    batch->List.mapWithIndex((i, (t: SendForm.StateLenses.state, _) as v) =>
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
      <Typography.Subtitle1>
        {reduceAmounts(batch->List.map(fst))->showCurrency->React.string}
      </Typography.Subtitle1>
    </View>
    <Transactions recipients showCurrency onAddCSVList onDelete />
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
