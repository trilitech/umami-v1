open ReactNative;
open Belt;

type inputType =
  | Selector
  | Text;

module BakerSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "selectorContent":
          style(
            ~height=44.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~flex=1.,
            (),
          ),
        "inner":
          style(~flex=1., ~flexDirection=`row, ~paddingLeft=20.->dp, ()),
        "address": style(~flexBasis=120.->dp, ~marginLeft=auto, ()),
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
};

module BakerInputTypeToogle = {
  let styles =
    Style.(
      StyleSheet.create({
        "inputTypeButton":
          style(~flexDirection=`row, ~alignItems=`center, ()),
        "inputTypeText":
          style(~color="#D8BC63", ~lineHeight=16., ~marginLeft=7.->dp, ()),
      })
    );

  [@react.component]
  let make = (~inputType, ~onPress) => {
    <TouchableOpacity style=styles##inputTypeButton onPress>
      {switch (inputType) {
       | Selector =>
         <>
           <Icons.Edit size=14. color="#D8BC63" />
           <Typography.ButtonSecondary style=styles##inputTypeText>
             "CUSTOM"->React.string
           </Typography.ButtonSecondary>
         </>
       | Text =>
         <>
           <Icons.List size=14. color="#D8BC63" />
           <Typography.ButtonSecondary style=styles##inputTypeText>
             "SEE LIST"->React.string
           </Typography.ButtonSecondary>
         </>
       }}
    </TouchableOpacity>;
  };
};

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
    })
  );

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error) => {
  let bakersRequest = StoreContext.Bakers.useLoad();

  let hasError = error->Option.isSome;

  let items =
    bakersRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault([||])
    ->Array.map(baker => {Selector.value: baker.address, label: baker.name});

  let (inputType, setInputType) = React.useState(_ => Selector);

  React.useEffect4(
    () => {
      if (inputType == Selector) {
        if (value == "") {
          // if input selector and no value, select first entry
          let firstItem = items->Array.get(0);
          firstItem->Common.Lib.Option.iter(item => item.value->handleChange);
        } else if (items->Array.size > 0
                   && !items->Array.some(item => item.value == value)) {
          // if input selector and value isn't in the item list : switch to input text
          setInputType(_ =>
            Text
          );
        };
      };
      None;
    },
    (value, items, inputType, setInputType),
  );

  let onPressInputType = _e => {
    setInputType(prevInputType => prevInputType == Selector ? Text : Selector);
    // reset value when switching inputType
    handleChange("");
  };

  <FormGroup style=styles##formGroup>
    <View style=styles##labelContainer>
      <FormLabel label hasError />
      <BakerInputTypeToogle inputType onPress=onPressInputType />
    </View>
    <View>
      {switch (inputType) {
       | Selector =>
         <Selector
           items
           onValueChange=handleChange
           selectedValue=value
           renderButton=BakerSelector.renderButton
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
