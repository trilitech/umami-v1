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
          ~color="rgba(255,255,255,0.8)",
          ~fontSize=16.,
          ~fontWeight=`_400,
          ~borderColor="rgba(255,255,255,0.6)",
          ~borderWidth=1.,
          ~borderRadius=5.,
          (),
        ),
      "inputError": style(~color="#f97977", ~borderColor="#f97977", ()),
      "inputSmall": style(~height=44.->dp, ()),
    })
  );

[@react.component]
let make =
    (~label, ~value, ~handleChange, ~error, ~keyboardType=?, ~small=false) => {
  let hasError = error->Belt.Option.isSome;
  <FormGroup small>
    <FormLabel label hasError small />
    <TextInput
      style=Style.(
        arrayOption([|
          Some(styles##input),
          hasError ? Some(styles##inputError) : None,
          small ? Some(styles##inputSmall) : None,
        |])
      )
      value
      onChange={(event: TextInput.changeEvent) =>
        handleChange(event.nativeEvent.text)
      }
      autoCapitalize=`none
      autoCorrect=false
      autoFocus=false
      ?keyboardType
    />
  </FormGroup>;
};