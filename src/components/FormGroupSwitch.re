open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup":
        style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
      "switchCmp": style(~height=10.->dp, ~width=20.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
    })
  );

[@react.component]
let make = (~label, ~value, ~handleChange, ~error) => {
  let hasError = error->Belt.Option.isSome;
  <FormGroup style=styles##formGroup small=false>
    <FormLabel label hasError />
    <SwitchNative
      value
      onValueChange=handleChange
      thumbColor="#000"
      trackColor={Switch.trackColor(
        ~_true="#FFF",
        ~_false="rgba(255,255,255,0.5)",
        (),
      )}
      style=styles##switchCmp
      thumbStyle=styles##switchThumb
    />
  </FormGroup>;
};
