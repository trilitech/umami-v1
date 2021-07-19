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
          ~zIndex=2,
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
        keyPopover={"inputMnemonicWord" ++ displayIndex->string_of_int}
        value
        handleChange
        error
        list=wordlists
        dropdownOnEmpty=false
        renderItem
        reversePositionPct=0.60
        keyExtractor
        renderLabel={renderLabel(displayIndex)}
        style=styles##input
        inputPaddingLeft={10. +. 17. +. 13.}
        itemHeight
        numItemsToDisplay
      />;
    });
};

module Dummy = {
  [@react.component]
  let make = () =>
    <View style=Style.(style(~flexBasis=40.->pct, ~flexGrow=1., ())) />;
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
