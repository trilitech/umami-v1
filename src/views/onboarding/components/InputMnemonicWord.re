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
      <Typography.Body2 colorStyle=`mediumEmphasis>
        word->React.string
      </Typography.Body2>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "input": style(~height=36.->dp, ()),
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

module Base = {
  [@react.component]
  let make =
    React.memo(
      (
        ~displayIndex: int,
        ~value: string,
        ~handleChange: string => unit,
        ~error: option(string),
      ) => {
      let wordlists =
        Bip39.wordlistsEnglish
        ->Array.keep(
            Js.String.startsWith(
              value->Js.String2.trim->Js.String2.toLowerCase,
            ),
          )
        ->Array.slice(~offset=0, ~len=12);

      <Autocomplete
        value
        handleChange
        error
        list=wordlists
        renderItem
        keyExtractor
        renderLabel={renderLabel(displayIndex)}
        style=styles##input
        inputPaddingLeft={10. +. 17. +. 13.}
        itemHeight
        numItemsToDisplay
      />;
    });
};

[@react.component]
let make =
    (
      ~arrayUpdateByIndex,
      ~getNestedFieldError,
      ~index,
      ~word,
      ~displayIndex=?,
      ~stateField,
      ~formField,
    ) => {
  let handleChange =
    React.useMemo2(
      () => {arrayUpdateByIndex(~field=stateField, ~index)},
      (index, stateField),
    );

  let error = getNestedFieldError(formField, index);

  <Base
    displayIndex={displayIndex->Option.getWithDefault(index)}
    value=word
    handleChange
    error
  />;
};
