open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "input":
        style(
          ~height=46.->dp,
          ~paddingVertical=10.->dp,
          ~paddingLeft=20.->dp,
          ~paddingRight=12.->dp,
          ~fontFamily="Avenir",
          ~fontSize=16.,
          ~fontWeight=`normal,
          ~borderWidth=1.,
          ~borderRadius=5.,
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
      ~hasError,
      ~keyboardType=?,
      ~onBlur=?,
      ~onFocus=?,
      ~onKeyPress=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~placeholder=?,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let theme = ThemeContext.useTheme();
  <TextInput
    ref=?inputRef
    style=Style.(
      arrayOption([|
        Some(styles##input),
        Some(
          style(
            ~color=theme.colors.textHighEmphasis,
            ~borderColor=theme.colors.borderMediumEmphasis,
            (),
          ),
        ),
        hasError
          ? Some(
              style(
                ~color=theme.colors.error,
                ~borderColor=theme.colors.error,
                (),
              ),
            )
          : None,
        styleFromProp,
      |])
    )
    placeholderTextColor={theme.colors.textDisabled}
    value
    onChange={(event: TextInput.changeEvent) =>
      onValueChange(event.nativeEvent.text)
    }
    ?onBlur
    ?onFocus
    ?onKeyPress
    ?textContentType
    ?secureTextEntry
    autoCapitalize=`none
    autoCorrect=false
    autoFocus=false
    ?keyboardType
    ?placeholder
  />;
};
