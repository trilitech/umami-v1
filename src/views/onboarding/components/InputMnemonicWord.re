open ReactNative;

let itemHeight = 26.;
let numItemsToDisplay = 8.;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~height=itemHeight->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~paddingLeft=(10. +. 17. +. 13.)->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~word) => {
    <View style=styles##itemContainer>
      <Typography.Body3 colorStyle=`mediumEmphasis>
        word->React.string
      </Typography.Body3>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "input":
        style(~paddingLeft=(10. +. 17. +. 13.)->dp, ~height=36.->dp, ()),
      "wordItemIndexContainer":
        style(
          ~position=`absolute,
          ~left=10.->dp,
          ~top=0.->dp,
          ~bottom=0.->dp,
          ~justifyContent=`center,
          (),
        ),
      "wordItemIndex": style(~width=17.->dp, ~textAlign=`right, ()),
    })
  );

let renderItem = (word: string) => <Item word />;

let keyExtractor = (word: string) => word;

let renderLabel = (displayIndex, displayError) => {
  <View style=styles##wordItemIndexContainer>
    <Typography.Subtitle1
      colorStyle={displayError ? `error : `mediumEmphasis}
      style=styles##wordItemIndex>
      {(displayIndex + 1)->string_of_int->React.string}
    </Typography.Subtitle1>
  </View>;
};

[@react.component]
let make = (~displayIndex, ~value, ~handleChange, ~error) => {
  let wordlists =
    Bip39.wordlistsEnglish
    ->Belt.Array.keep(Js.String.startsWith(value))
    ->Belt.Array.slice(~offset=0, ~len=12);

  <Autocomplete
    value
    handleChange
    error
    list=wordlists
    renderItem
    keyExtractor
    renderLabel={renderLabel(displayIndex)}
    style=styles##input
    itemHeight
    numItemsToDisplay
  />;
};
