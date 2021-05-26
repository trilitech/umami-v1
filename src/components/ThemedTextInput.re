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

let borderWidth = 1.;
let paddingVertical = 12.;
let paddingLeft = 22.;
let paddingRight = 14.;
let clearIconSize = 28.;

let styles =
  Style.(
    StyleSheet.create({
      "clearMargin": style(~paddingRight=35.->dp, ()),
      "clearBtn":
        style(
          ~right=5.->dp,
          ~top=50.->pct,
          ~marginTop=(-. clearIconSize /. 2.)->dp,
          ~position=`absolute,
          (),
        ),
      "input":
        style(
          ~height=44.->dp,
          ~fontFamily="SourceSansPro",
          ~fontSize=16.,
          ~fontWeight=`normal,
          ~borderWidth,
          ~borderRadius=4.,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~inputRef=?,
      ~value,
      ~onValueChange,
      ~hasError=false,
      ~keyboardType=?,
      ~onBlur=?,
      ~onClear=?,
      ~onFocus=?,
      ~onKeyPress=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~placeholder=?,
      ~onSubmitEditing=?,
      ~disabled=false,
      ~paddingVertical=paddingVertical,
      ~paddingLeft=paddingLeft,
      ~paddingRight=paddingRight,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let theme = ThemeContext.useTheme();
  let (isFocused, setIsFocused) = React.useState(_ => false);

  <>
    {onClear
     ->ReactUtils.mapOpt(onClear => {
         <View style=styles##clearBtn>
           <IconButton
             onPress={_ => onClear()}
             icon=Icons.Close.build
             size=clearIconSize
           />
         </View>
       })
     ->ReactUtils.onlyWhen(value != "")}
    <TextInput
      ref=?inputRef
      style=Style.(
        arrayOption([|
          Some(styles##input),
          Some(
            style(
              ~color=theme.colors.textHighEmphasis,
              ~backgroundColor=theme.colors.background,
              ~borderColor=theme.colors.borderMediumEmphasis,
              ~paddingVertical=(paddingVertical -. borderWidth)->dp,
              ~paddingLeft=(paddingLeft -. borderWidth)->dp,
              ~paddingRight=(paddingRight -. borderWidth)->dp,
              (),
            ),
          ),
          isFocused && !disabled
            ? Some(
                style(
                  ~borderColor=theme.colors.borderPrimary,
                  ~borderWidth=2.,
                  ~paddingVertical=(paddingVertical -. 2.)->dp,
                  ~paddingLeft=(paddingLeft -. 2.)->dp,
                  ~paddingRight=(paddingRight -. 2.)->dp,
                  (),
                ),
              )
            : None,
          hasError
            ? Some(
                style(
                  ~color=theme.colors.error,
                  ~borderColor=theme.colors.error,
                  ~borderWidth=2.,
                  ~paddingVertical=(paddingVertical -. 2.)->dp,
                  ~paddingLeft=(paddingLeft -. 2.)->dp,
                  ~paddingRight=(paddingRight -. 2.)->dp,
                  (),
                ),
              )
            : None,
          disabled
            ? Some(style(~color=theme.colors.textDisabled, ())) : None,
          value != "" && onClear != None ? Some(styles##clearMargin) : None,
          styleFromProp,
        |])
      )
      placeholderTextColor={theme.colors.textDisabled}
      value
      onChange={(event: TextInput.changeEvent) =>
        onValueChange(event.nativeEvent.text)
      }
      onFocus={event => {
        setIsFocused(_ => true);
        onFocus->Lib.Option.iter(onFocus => onFocus(event));
      }}
      onBlur={event => {
        setIsFocused(_ => false);
        onBlur->Lib.Option.iter(onBlur => onBlur(event));
      }}
      ?onKeyPress
      ?textContentType
      ?secureTextEntry
      autoCapitalize=`none
      autoCorrect=false
      autoFocus=false
      ?keyboardType
      ?placeholder
      ?onSubmitEditing
      editable={!disabled}
    />
  </>;
};

[@bs.send]
external measureInWindow:
  (
    TextInput.element,
    (~x: float, ~y: float, ~width: float, ~height: float) => unit
  ) =>
  unit =
  "measureInWindow";
