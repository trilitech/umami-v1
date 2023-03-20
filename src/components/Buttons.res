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

let styles = {
  open Style
  StyleSheet.create({
    "primary": style(~borderRadius=4., ()),
    "content": FormStyles.flexAlignedRow(~flex=1., ()),
    "chevronRight": style(~transform=[rotate(~rotate=270.->deg)], ()),
    "chevronUp": style(~transform=[rotate(~rotate=180.->deg)], ()),
    "pressable": style(
      ~flex=1.,
      ~paddingHorizontal=17.->dp,
      ~minWidth=95.->dp,
      ~minHeight=34.->dp,
      ~maxHeight=34.->dp,
      ~alignItems=#center,
      ~justifyContent=#center,
      ~borderRadius=4.,
      (),
    ),
    "loader": style(~position=#absolute, ()),
    "thin": style(~maxHeight=27.->dp, ~minHeight=27.->dp, ~justifyContent=#center, ()),
  })
}

module FormBase = {
  @react.component
  let make = (
    ~onPress,
    ~disabled=false,
    ~loading=false,
    ~isPrimary=false,
    ~vStyle=?,
    ~style=?,
    ~children,
  ) => {
    let theme = ThemeContext.useTheme()

    let module(ThemedPressableComp: ThemedPressable.T) = isPrimary
      ? module(ThemedPressable.Primary)
      : module(ThemedPressable.Base)

    <View style=?vStyle>
      <ThemedPressableComp
        style={Style.arrayOption([Some(styles["pressable"]), style])}
        onPress
        disabled={disabled || loading}
        accessibilityRole=#button>
        {loading
          ? <ActivityIndicator
              animating=true
              size={18.->ActivityIndicator.Size.exact}
              color={isPrimary
                ? theme.colors.primaryIconMediumEmphasis
                : theme.colors.iconMediumEmphasis}
              style={styles["loader"]}
            />
          : React.null}
        <View
          style={
            open Style
            array([ReactUtils.visibleOn(!loading), styles["content"]])
          }>
          children
        </View>
      </ThemedPressableComp>
    </View>
  }
}

module Form = {
  @react.component
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) =>
    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{disabled->Option.flatMap(disabled =>
          disabled ? Some(#disabled) : Some(#highEmphasis)
        )}>
        {text->React.string}
      </Typography.ButtonPrimary>
    </FormBase>
}

module FormPrimary = {
  @react.component
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?, ~colorStyle=?) => {
    let colorStyle = switch colorStyle {
    | None => disabled->Option.flatMap(disabled => disabled ? Some(#disabled) : Some(#highEmphasis))

    | c => c
    }

    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonPrimary ?fontSize ?colorStyle>
        {text->React.string}
      </Typography.ButtonPrimary>
    </FormBase>
  }
}

module RightArrowButton = {
  let localStyles = {
    open Style
    StyleSheet.create({
      "content": FormStyles.flexAlignedRow(~justifyContent=#spaceBetween, ~flex=1., ()),
      "button": style(
        ~display=#flex,
        ~justifyContent=#spaceBetween,
        ~flexDirection=#row,
        ~paddingHorizontal=0.->dp,
        (),
      ),
    })
  }

  @react.component
  let make = (~text, ~onPress, ~disabled=false, ~stateIcon=?, ~style as styleArg=?) => {
    let theme = ThemeContext.useTheme()

    <FormBase
      disabled
      onPress
      style={
        open Style
        arrayOption([styleArg, localStyles["button"]->Some])
      }>
      <View style={localStyles["content"]}>
        <Typography.ButtonSecondary colorStyle=?{disabled ? Some(#disabled) : None} fontSize=14.>
          {text->React.string}
        </Typography.ButtonSecondary>
        <View style={FormStyles.flexAlignedRow()}>
          {stateIcon->ReactUtils.opt}
          <Icons.ChevronDown
            color={disabled ? theme.colors.iconDisabled : theme.colors.iconMediumEmphasis}
            style={styles["chevronRight"]}
            size=28.
          />
        </View>
      </View>
    </FormBase>
  }
}

module FormSecondary = {
  @react.component
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) =>
    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonTernary
        ?fontSize
        colorStyle=?{disabled->Option.flatMap(disabled =>
          disabled ? Some(#disabled) : Some(#highEmphasis)
        )}>
        {text->React.string}
      </Typography.ButtonTernary>
    </FormBase>
}

module SubmitPrimary = {
  @react.component
  let make = (
    ~text,
    ~onPress,
    ~disabled=false,
    ~disabledLook=false,
    ~danger=false,
    ~loading=?,
    ~fontSize=?,
    ~style as argStyle=?,
  ) => {
    let theme = ThemeContext.useTheme()

    <FormBase
      onPress
      isPrimary=true
      disabled
      ?loading
      vStyle={
        open Style
        arrayOption([
          Some(styles["primary"]),
          Some(
            style(
              ~backgroundColor=danger ? theme.colors.error : theme.colors.primaryButtonBackground,
              (),
            ),
          ),
          argStyle,
        ])
      }>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle={disabled || disabledLook ? #filledBackDisabled : #filledBackHighEmphasis}>
        {text->React.string}
      </Typography.ButtonPrimary>
    </FormBase>
  }
}

module SubmitSecondary = {
  @react.component
  let make = (
    ~text,
    ~onPress,
    ~disabled=false,
    ~loading=?,
    ~fontSize=?,
    ~borderWidth=2.,
    ~style as styleArg=?,
  ) => {
    let theme = ThemeContext.useTheme()

    <FormBase
      onPress
      disabled
      ?loading
      vStyle=?styleArg
      style={
        open Style
        style(
          ~borderWidth,
          ~borderColor=disabled ? theme.colors.borderDisabled : theme.colors.borderHighEmphasis,
          (),
        )
      }>
      <Typography.ButtonPrimary ?fontSize colorStyle={disabled ? #disabled : #highEmphasis}>
        {text->React.string}
      </Typography.ButtonPrimary>
    </FormBase>
  }
}

module SubmitTertiary = {
  @react.component
  let make = (~text, ~onPress, ~disabled=false, ~loading=?, ~fontSize=?, ~style as styleArg=?) => {
    let theme = ThemeContext.useTheme()

    <FormBase
      onPress
      disabled
      ?loading
      vStyle=?styleArg
      style={
        open Style
        style(~borderWidth=1., ~borderColor=theme.colors.borderMediumEmphasis, ())
      }>
      <Typography.ButtonPrimary colorStyle=#mediumEmphasis ?fontSize>
        {text->React.string}
      </Typography.ButtonPrimary>
    </FormBase>
  }
}
