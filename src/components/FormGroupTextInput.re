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
      "decoration":
        style(
          ~display=`flex,
          ~alignItems=`center,
          ~position=`absolute,
          ~marginTop=auto,
          ~marginBottom=auto,
          ~top=0.->dp,
          ~bottom=0.->dp,
          ~right=10.->dp,
          (),
        ),
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
      ~onBlur=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~decoration: option((~style: Style.t) => React.element)=?,
      ~style: option(ReactNative.Style.t)=?,
    ) => {
  let hasError = error->Belt.Option.isSome;
  <FormGroup ?style>
    <FormLabel label hasError style=styles##label />
    <View>
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
        ?onBlur
        ?textContentType
        ?secureTextEntry
        autoCapitalize=`none
        autoCorrect=false
        autoFocus=false
        ?keyboardType
      />
      {decoration->ReactUtils.mapOpt(deco => deco(~style=styles##decoration))}
    </View>
  </FormGroup>;
};
