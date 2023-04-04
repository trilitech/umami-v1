/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

let borderWidth = 1.
let paddingVertical = 12.
let paddingLeft = 22.
let paddingRight = 14.
let clearIconSize = 28.

let styles = {
  open Style
  StyleSheet.create({
    "clearMargin1": style(~paddingRight=35.->dp, ()),
    "clearMargin2": style(~paddingRight=61.->dp, ()),
    "clearBtn": style(
      ~right=5.->dp,
      ~top=50.->pct,
      ~marginTop=(-.clearIconSize /. 2.)->dp,
      ~position=#absolute,
      ~flexDirection=#row,
      (),
    ),
    "container": style(~borderWidth=2., ~borderRadius=4., ~justifyContent=#center, ()),
    "containerHeight": style(~height=44.->dp, ()),
    "input": style(~fontFamily="SourceSansPro", ~fontSize=16., ~fontWeight=#normal, ~flex=1., ()),
    "multiline": style(~height=auto, ()),
    "icon": style(~paddingRight=6.->dp, ~paddingLeft=6.->dp, ()),
  })
}

let makeFrameStyle = (
  ~theme: ThemeContext.theme,
  ~numberOfLines,
  ~multiline,
  ~icon,
  ~isFocused,
  ~disabled,
  ~hasError,
  ~value,
  ~onClear,
  ~onRemove,
  ~styleFromProp,
  ~paddingVertical,
  ~paddingLeft,
  ~paddingRight,
) => {
  open Style
  arrayOption([
    styles["container"]->Some,
    numberOfLines == None ? styles["containerHeight"]->Some : None,
    multiline ? Some(styles["multiline"]) : None,
    Some(
      style(
        ~backgroundColor=theme.colors.background,
        ~borderColor=theme.colors.borderMediumEmphasis,
        ~paddingVertical=(paddingVertical -. borderWidth)->dp,
        ~paddingRight=(paddingRight -. borderWidth)->dp,
        (),
      ),
    ),
    switch icon {
    | Some(_) => None
    | None => Some(style(~paddingLeft=(paddingLeft -. borderWidth)->dp, ()))
    },
    isFocused && !disabled
      ? Some(style(~borderColor=theme.colors.borderPrimary, ~borderWidth=2., ()))
      : None,
    hasError ? Some(style(~borderColor=theme.colors.error, ~borderWidth=2., ())) : None,
    value != "" && onClear != None
      ? onRemove != None ? Some(styles["clearMargin2"]) : Some(styles["clearMargin1"])
      : onRemove != None
      ? Some(styles["clearMargin1"])
      : None,
    styleFromProp,
    disabled
      ? Some(
          style(
            ~color=theme.colors.textDisabled,
            ~backgroundColor=theme.colors.stateDisabled,
            ~borderWidth=0.,
            ~paddingVertical=paddingVertical->dp,
            (),
          ),
        )
      : None,
  ])
}
@react.component
let make = (
  ~inputRef=?,
  ~value,
  ~onValueChange,
  ~hasError=false,
  ~keyboardType=?,
  ~onBlur=?,
  ~onClear=?,
  ~onRemove=?,
  ~onFocus=?,
  ~onKeyPress=?,
  ~textContentType=?,
  ~secureTextEntry=?,
  ~placeholder=?,
  ~onSubmitEditing=?,
  ~disabled=false,
  ~icon=?,
  ~multiline=false,
  ~numberOfLines=?,
  ~paddingVertical=paddingVertical,
  ~paddingLeft=paddingLeft,
  ~paddingRight=paddingRight,
  ~style as styleFromProp: option<ReactNative.Style.t>=?,
) => {
  let theme = ThemeContext.useTheme()
  let (isFocused, setIsFocused) = React.useState(_ => false)

  let frameStyle = makeFrameStyle(
    ~theme,
    ~numberOfLines,
    ~multiline,
    ~icon,
    ~isFocused,
    ~disabled,
    ~hasError,
    ~value,
    ~onClear,
    ~onRemove,
    ~styleFromProp,
    ~paddingVertical,
    ~paddingLeft,
    ~paddingRight,
  )

  <View style=frameStyle>
    {onRemove != None || onClear != None
      ? <View style={styles["clearBtn"]}>
          {onRemove->ReactUtils.mapOpt(onRemove =>
            <IconButton onPress={_ => onRemove()} icon=Icons.Delete.build size=clearIconSize />
          )}
          {onClear
          ->ReactUtils.mapOpt(onClear =>
            <IconButton onPress={_ => onClear()} icon=Icons.Close.build size=clearIconSize />
          )
          ->ReactUtils.onlyWhen(value != "")}
        </View>
      : React.null}
    {switch icon {
    | Some(build) =>
      build(~color=?Some(theme.colors.textMediumEmphasis), ~style=?Some(styles["icon"]), ~size=24.)
    | None => React.null
    }}
    <TextInput
      ref=?inputRef
      style={
        open Style
        array([
          styles["input"],
          style(
            ~color=hasError ? theme.colors.error : theme.colors.textHighEmphasis,
            ~fontWeight=disabled ? #bold : #normal,
            (),
          ),
        ])
      }
      placeholderTextColor=theme.colors.textDisabled
      value
      onChange={(event: TextInput.changeEvent) => onValueChange(event.nativeEvent.text)}
      onFocus={event => {
        setIsFocused(_ => true)
        onFocus->Option.iter(onFocus => onFocus(event))
      }}
      onBlur={event => {
        setIsFocused(_ => false)
        onBlur->Option.iter(onBlur => onBlur(event))
      }}
      ?onKeyPress
      ?textContentType
      ?secureTextEntry
      autoCapitalize=#none
      autoCorrect=false
      autoFocus=false
      ?keyboardType
      ?placeholder
      multiline
      ?numberOfLines
      ?onSubmitEditing
      editable={!disabled}
    />
  </View>
}

@send
external measureInWindow: (
  TextInput.element,
  (~x: float, ~y: float, ~width: float, ~height: float) => unit,
) => unit = "measureInWindow"
