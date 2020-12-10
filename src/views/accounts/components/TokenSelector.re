open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "selector":
        style(
          ~zIndex=4,
          ~alignSelf=`flexStart,
          ~minWidth=320.->dp,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          (),
        ),
      "selectorContent":
        style(
          ~height=42.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "spacer": style(~height=6.->dp, ()),
    })
  );

let xtzToken: Token.t = {
  address: "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
  alias: I18n.t#tezos,
  symbol: I18n.t#xtz,
};

module TokenItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "inner":
          style(
            ~height=22.->dp,
            ~marginLeft=14.->dp,
            ~marginRight=10.->dp,
            ~flex=1.,
            ~flexDirection=`row,
            ~justifyContent=`spaceBetween,
            (),
          ),
        "titleContainer": style(~flexDirection=`row, ~alignItems=`center, ()),
        "icon": style(~marginRight=10.->dp, ()),
      })
    );

  [@react.component]
  let make = (~token: Token.t) => {
    <View style=styles##inner>
      <View style=styles##titleContainer>
        {token.symbol == xtzToken.symbol
           ? <Icons.Tezos
               size=20.
               color=Theme.colorDarkMediumEmphasis
               style=styles##icon
             />
           : <Icons.Token
               size=20.
               color=Theme.colorDarkMediumEmphasis
               style=styles##icon
             />}
        <Typography.Subtitle2>
          token.alias->React.string
        </Typography.Subtitle2>
      </View>
      <Typography.Body1> token.symbol->React.string </Typography.Body1>
    </View>;
  };
};

let renderButton = (selectedToken: option(Token.t)) =>
  <View style=styles##selectorContent>
    {selectedToken->Belt.Option.mapWithDefault(<LoadingView />, token =>
       <TokenItem token />
     )}
  </View>;

let renderItem = (token: Token.t) => <TokenItem token />;

[@react.component]
let make = (~selectedToken, ~setSelectedToken, ~style as styleProp=?) => {
  let tokensRequest = StoreContext.Tokens.useLoad();

  let items =
    tokensRequest
    ->ApiRequest.getDoneOk
    ->Belt.Option.mapWithDefault([||], Belt.Map.String.valuesToArray);

  let onValueChange = newValue => {
    setSelectedToken(_ =>
      newValue == xtzToken.address ? None : Some(newValue)
    );
  };

  items->Belt.Array.size > 0
    ? <Selector
        style=Style.(arrayOption([|Some(styles##selector), styleProp|]))
        items
        getItemValue={token => token.address}
        renderButton
        onValueChange
        renderItem
        selectedValue=?selectedToken
        noneItem=xtzToken
      />
    : React.null;
};
