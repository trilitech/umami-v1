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
          ~color=Theme.colorDarkHighEmphasis,
          ~fontSize=16.,
          ~fontWeight=`normal,
          ~borderColor=Theme.colorDarkMediumEmphasis,
          ~borderWidth=1.,
          ~borderRadius=5.,
          (),
        ),
      "inputError":
        style(
          ~color=Theme.colorDarkError,
          ~borderColor=Theme.colorDarkError,
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
  <TextInput
    ref=?inputRef
    style=Style.(
      arrayOption([|
        Some(styles##input),
        hasError ? Some(styles##inputError) : None,
        styleFromProp,
      |])
    )
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
