open ReactNative;
open Common;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~marginTop=30.->dp, ()),
      "list":
        style(
          ~minHeight=300.->dp,
          ~maxHeight=400.->dp,
          ~borderWidth=1.,
          ~borderRadius=4.,
          (),
        ),
      "listLabel": style(~marginBottom=4.->dp, ()),
      "amount": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
      "summary": style(~marginTop=11.->dp, ()),
      "row":
        style(
          ~paddingVertical=12.->dp,
          ~paddingLeft=28.->dp,
          ~paddingRight=43.->dp,
          (),
        ),
      "notFirstRow": style(~borderTopWidth=1., ()),
      "num": style(~position=`absolute, ~left=10.->dp, ~top=10.->dp, ()),
      "moreButton":
        style(~position=`absolute, ~top=21.->dp, ~right=9.->dp, ()),
    })
  );

let buildAmount = amount => {
  AccountSelector.Amount(
    <Typography.Body1 fontWeightStyle=`bold style=styles##amount>
      {I18n.t#xtz_amount(amount)->React.string}
    </Typography.Body1>,
  );
};

let computeTotal = batch =>
  batch->Belt.List.reduce(0., (acc, t: SendForm.StateLenses.state) =>
    acc +. float_of_string(t.amount)
  );

module Item = {
  [@react.component]
  let make = (~i, ~recipient, ~amount) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let theme: ThemeContext.theme = ThemeContext.useTheme();
    <View
      key={string_of_int(i)}
      style={ReactUtils.styles(
        styles##row
        @: Style.(style(~borderColor=theme.colors.borderDisabled, ()))
        @$? Lib.Option.onlyIf(i != 0, () => styles##notFirstRow),
      )}>
      <Typography.Subtitle1 colorStyle=`mediumEmphasis style=styles##num>
        {(i + 1)->string_of_int->React.string}
      </Typography.Subtitle1>
      <AccountSelector.AccountItem
        account={
          address: recipient,
          alias:
            recipient
            ->AliasHelpers.getAliasFromAddress(aliases)
            ->Belt.Option.getWithDefault(""),
        }
        showAmount={buildAmount(amount)}
      />
      <TouchableOpacity style=styles##moreButton onPress={_ => ()}>
        <Icons.More size=24. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
    </View>;
  };
};

module Transactions = {
  [@react.component]
  let make = (~recipients) => {
    <View style=styles##container>
      <Typography.Overline2 style=styles##listLabel>
        I18n.label#transactions->React.string
      </Typography.Overline2>
      <View style=styles##list>
        {{
           recipients
           ->Belt.List.toArray
           ->Belt.Array.mapWithIndex((i, (recipient, amount)) => {
               <Item i recipient amount />
             });
         }
         ->React.array}
      </View>
    </View>;
  };
};

[@react.component]
let make = (~back, ~onSubmitBatch, ~batch: list(SendForm.StateLenses.state)) => {
  let theme: ThemeContext.theme = ThemeContext.useTheme();
  let recipients = batch->Belt.List.map(t => (t.recipient, t.amount));
  <>
    {<TouchableOpacity onPress={_ => back()} style=FormStyles.topLeftButton>
       <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
     </TouchableOpacity>}
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
        {I18n.t#xtz_amount(computeTotal(batch)->Js.Float.toString)
         ->React.string}
      </Typography.Subtitle1>
    </View>
    <Transactions recipients />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.btn#batch_submit
        onPress={_ => onSubmitBatch(batch)}
      />
    </View>
  </>;
};
