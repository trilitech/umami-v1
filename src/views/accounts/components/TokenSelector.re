open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "selector":
        style(
          ~zIndex=4,
          ~alignSelf=`flexStart,
          ~maxWidth=320.->dp,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          (),
        ),
      "selectorContent":
        style(
          ~height=40.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "selectorDropdown": style(~minWidth=275.->dp, ~left=auto, ()),
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
    let theme = ThemeContext.useTheme();
    <View style=styles##inner>
      <View style=styles##titleContainer>
        {token.symbol == xtzToken.symbol
           ? <Icons.Tezos
               size=20.
               color={theme.colors.iconMediumEmphasis}
               style=styles##icon
             />
           : <Icons.Token
               size=20.
               color={theme.colors.iconMediumEmphasis}
               style=styles##icon
             />}
        <Typography.Subtitle2>
          token.alias->React.string
        </Typography.Subtitle2>
      </View>
      <Typography.Body1 colorStyle=`mediumEmphasis>
        token.symbol->React.string
      </Typography.Body1>
    </View>;
  };
};

let renderButton = (selectedToken: option(Token.t), _hasError) =>
  <View style=styles##selectorContent>
    {selectedToken->Option.mapWithDefault(<LoadingView />, token =>
       <View style=TokenItem.styles##inner>
         <Typography.Body1> token.symbol->React.string </Typography.Body1>
       </View>
     )}
  </View>;

let renderItem = (token: Token.t) => <TokenItem token />;

[@react.component]
let make =
    (
      ~selectedToken,
      ~setSelectedToken,
      ~style as styleProp=?,
      ~renderButton=renderButton,
    ) => {
  let tokens = StoreContext.Tokens.useGetAll();

  let items = tokens->Map.String.valuesToArray;

  let onValueChange = newValue => {
    setSelectedToken(newValue == xtzToken.address ? None : Some(newValue));
  };

  items->Array.size > 0
    ? <Selector
        style=Style.(arrayOption([|Some(styles##selector), styleProp|]))
        dropdownStyle=styles##selectorDropdown
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
