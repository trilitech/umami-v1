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

let styles =
  Style.(
    StyleSheet.create({
      "primary": style(~borderRadius=4., ()),
      "content":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~flex=1.,
          ~alignItems=`center,
          (),
        ),
      "pressable":
        style(
          ~flex=1.,
          ~paddingHorizontal=17.->dp,
          ~minWidth=95.->dp,
          ~minHeight=34.->dp,
          ~maxHeight=34.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderRadius=4.,
          (),
        ),
      "loader": style(~position=`absolute, ()),
    })
  );

module FormBase = {
  [@react.component]
  let make =
      (
        ~onPress,
        ~disabled=false,
        ~loading=false,
        ~isPrimary=false,
        ~vStyle=?,
        ~style=?,
        ~children,
      ) => {
    let theme = ThemeContext.useTheme();

    let (module ThemedPressableComp): (module ThemedPressable.T) =
      isPrimary
        ? (module ThemedPressable.Primary) : (module ThemedPressable.Base);

    <View style=?vStyle>
      <ThemedPressableComp
        style={Style.arrayOption([|Some(styles##pressable), style|])}
        onPress
        disabled={disabled || loading}
        accessibilityRole=`button>
        {loading
           ? <ActivityIndicator
               animating=true
               size={18.->Style.dp}
               color={
                 isPrimary
                   ? theme.colors.primaryIconMediumEmphasis
                   : theme.colors.iconMediumEmphasis
               }
               style=styles##loader
             />
           : React.null}
        <View
          style=Style.(
            array([|ReactUtils.visibleOn(!loading), styles##content|])
          )>
          children
        </View>
      </ThemedPressableComp>
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module FormPrimary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module RightArrowButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "chevron": style(~transform=[|rotate(~rotate=270.->deg)|], ()),
        "content":
          style(
            ~display=`flex,
            ~justifyContent=`spaceBetween,
            ~alignItems=`center,
            ~flexDirection=`row,
            ~flex=1.,
            (),
          ),
        "button":
          style(
            ~display=`flex,
            ~justifyContent=`spaceBetween,
            ~flexDirection=`row,
            ~paddingHorizontal=0.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~text, ~onPress, ~style as styleArg=?) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress style=Style.(arrayOption([|styleArg, styles##button->Some|]))>
      <View style=styles##content>
        <Typography.ButtonSecondary fontSize=14.>
          text->React.string
        </Typography.ButtonSecondary>
        <Icons.ChevronDown
          color={theme.colors.iconMediumEmphasis}
          style={styles##chevron}
          size=28.
        />
      </View>
    </FormBase>;
  };
};

module FormSecondary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
      <Typography.ButtonTernary
        ?fontSize
        colorStyle=?{
          disabled->Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonTernary>
    </FormBase>;
  };
};

module SubmitPrimary = {
  [@react.component]
  let make =
      (
        ~text,
        ~onPress,
        ~disabled=false,
        ~disabledLook=false,
        ~danger=false,
        ~loading=?,
        ~fontSize=?,
        ~style as argStyle=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      isPrimary=true
      disabled
      ?loading
      vStyle=Style.(
        arrayOption([|
          Some(styles##primary),
          Some(
            style(
              ~backgroundColor=
                danger
                  ? theme.colors.error : theme.colors.primaryButtonBackground,
              (),
            ),
          ),
          argStyle,
        |])
      )>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle={
          disabled || disabledLook
            ? `filledBackDisabled : `filledBackHighEmphasis
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module SubmitSecondary = {
  [@react.component]
  let make =
      (
        ~text,
        ~onPress,
        ~disabled=false,
        ~loading=?,
        ~fontSize=?,
        ~borderWidth=1.,
        ~style as styleArg=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      disabled
      ?loading
      vStyle=?styleArg
      style=Style.(
        style(~borderWidth, ~borderColor=theme.colors.borderHighEmphasis, ())
      )>
      <Typography.ButtonPrimary ?fontSize>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module SubmitTertiary = {
  [@react.component]
  let make =
      (
        ~text,
        ~onPress,
        ~disabled=false,
        ~loading=?,
        ~fontSize=?,
        ~style as styleArg=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      disabled
      ?loading
      vStyle=?styleArg
      style=Style.(
        style(
          ~borderWidth=1.,
          ~borderColor=theme.colors.borderMediumEmphasis,
          (),
        )
      )>
      <Typography.ButtonPrimary colorStyle=`mediumEmphasis ?fontSize>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};
