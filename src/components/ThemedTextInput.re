open ReactNative;

let borderWidth = 1.;
let paddingVertical = 12.;
let paddingLeft = 22.;
let paddingRight = 14.;

let styles =
  Style.(
    StyleSheet.create({
      "input":
        style(
          ~height=46.->dp,
          ~paddingVertical=(paddingVertical -. borderWidth)->dp,
          ~paddingLeft=(paddingLeft -. borderWidth)->dp,
          ~paddingRight=(paddingRight -. borderWidth)->dp,
          ~fontFamily="EBGaramond",
          ~fontSize=16.,
          ~fontWeight=`normal,
          ~borderWidth,
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
      ~disabled=false,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let theme = ThemeContext.useTheme();
  let (isFocused, setIsFocused) = React.useState(_ => false);
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
        disabled ? Some(style(~color=theme.colors.textDisabled, ())) : None,
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
      onFocus->Common.Lib.Option.iter(onFocus => onFocus(event));
    }}
    onBlur={event => {
      setIsFocused(_ => false);
      onBlur->Common.Lib.Option.iter(onBlur => onBlur(event));
    }}
    ?onKeyPress
    ?textContentType
    ?secureTextEntry
    autoCapitalize=`none
    autoCorrect=false
    autoFocus=false
    ?keyboardType
    ?placeholder
    editable={!disabled}
  />;
};
