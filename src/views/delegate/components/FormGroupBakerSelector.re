open ReactNative;
open Belt;

type inputStyle =
  | Selector
  | Text;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=10, ()),
      "labelContainer":
        style(
          ~marginBottom=6.->dp,
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          ~alignItems=`flexEnd,
          (),
        ),
      "selectorContent":
        style(
          ~height=44.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "inner": style(~flex=1., ~flexDirection=`row, ~paddingLeft=20.->dp, ()),
      "address": style(~flexBasis=120.->dp, ~marginLeft=auto, ()),
      "inputStyleButton": style(~flexDirection=`row, ~alignItems=`center, ()),
      "inputStyleText":
        style(~color="#D8BC63", ~lineHeight=16., ~marginLeft=7.->dp, ()),
    })
  );

let renderButton = (selectedItem: option(Selector.item)) =>
  <View style=styles##selectorContent>
    {selectedItem->Option.mapWithDefault(<LoadingView />, item =>
       <View style=styles##inner>
         <Typography.Subtitle2>
           item.label->React.string
         </Typography.Subtitle2>
         <Typography.Body1
           colorStyle=`mediumEmphasis numberOfLines=1 style=styles##address>
           item.value->React.string
         </Typography.Body1>
       </View>
     )}
  </View>;

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error) => {
  let bakersRequest = DelegateApiRequest.useGetBakers();

  let hasError = error->Option.isSome;

  let items =
    bakersRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault([||])
    ->Array.map(baker => {Selector.value: baker.address, label: baker.name});

  let (inputStyle, setInputStyle) = React.useState(_ => Selector);

  React.useEffect4(
    () => {
      if (inputStyle == Selector) {
        if (value == "") {
          let firstItem = items->Array.get(0);
          firstItem->Common.Lib.Option.iter(item => item.value->handleChange);
        } else if (items->Array.size > 0
                   && !items->Array.some(item => item.value == value)) {
          setInputStyle(_ => Text);
        };
      };
      None;
    },
    (value, items, inputStyle, setInputStyle),
  );

  let onPressInputStyle = _e => {
    setInputStyle(prevInputStyle =>
      prevInputStyle == Selector ? Text : Selector
    );
    handleChange("");
  };

  <FormGroup style=styles##formGroup>
    <View style=styles##labelContainer>
      <FormLabel label hasError />
      <TouchableOpacity
        style=styles##inputStyleButton onPress=onPressInputStyle>
        {switch (inputStyle) {
         | Selector =>
           <>
             <Icons.Edit size=14. color="#D8BC63" />
             <Typography.ButtonSecondary style=styles##inputStyleText>
               "CUSTOM"->React.string
             </Typography.ButtonSecondary>
           </>
         | Text =>
           <>
             <Icons.List size=14. color="#D8BC63" />
             <Typography.ButtonSecondary style=styles##inputStyleText>
               "SEE LIST"->React.string
             </Typography.ButtonSecondary>
           </>
         }}
      </TouchableOpacity>
    </View>
    <View>
      {switch (inputStyle) {
       | Selector =>
         <Selector
           items
           onValueChange=handleChange
           selectedValue=value
           renderButton
           renderItem=AccountSelector.renderItem
         />
       | Text =>
         <ThemedTextInput
           value
           onValueChange=handleChange
           hasError
           placeholder={js|Enter baker's tz1 address|js}
         />
       }}
    </View>
  </FormGroup>;
};
