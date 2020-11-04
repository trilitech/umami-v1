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
      "label": style(~marginBottom=6.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~handleChange,
      ~error,
      ~keyboardType=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~style: option(ReactNative.Style.t)=?,
    ) => {
  let hasError = error->Belt.Option.isSome;
  <FormGroup ?style>
    <FormLabel label hasError style=styles##label />
    <TextInput
      style=Style.(
        arrayOption([|
          Some(styles##input),
          hasError ? Some(styles##inputError) : None,
        |])
      )
      value
      onChange={(event: TextInput.changeEvent) =>
        handleChange(event.nativeEvent.text)
      }
      ?textContentType
      ?secureTextEntry
      autoCapitalize=`none
      autoCorrect=false
      autoFocus=false
      ?keyboardType
    />
  </FormGroup>;
};
