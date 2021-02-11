open UmamiCommon;
open ReactNative;

type inputType =
  | Selector
  | Text;

module BakerSelector = {
  module BakerItem = {
    let styles =
      Style.(
        StyleSheet.create({
          "inner":
            style(
              ~height=46.->dp,
              ~marginHorizontal=20.->dp,
              ~justifyContent=`spaceBetween,
              (),
            ),
        })
      );

    [@react.component]
    let make = (~baker: Delegate.t) => {
      <View style=styles##inner>
        <Typography.Subtitle2> baker.name->React.string </Typography.Subtitle2>
        <Typography.Address> baker.address->React.string </Typography.Address>
      </View>;
    };
  };

  let styles =
    Style.(
      StyleSheet.create({
        "selectorContent":
          style(
            ~height=40.->dp,
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

  let renderButton = (selectedBaker: option(Delegate.t), hasError) =>
    <View style=styles##selectorContent>
      {selectedBaker->Option.mapWithDefault(<LoadingView />, baker =>
         <View style=styles##inner>
           <Typography.Subtitle2 colorStyle=?{hasError ? Some(`error) : None}>
             baker.name->React.string
           </Typography.Subtitle2>
           <Typography.Address
             colorStyle=?{hasError ? Some(`error) : None}
             numberOfLines=1
             style=styles##address>
             baker.address->React.string
           </Typography.Address>
         </View>
       )}
    </View>;

  let renderItem = (baker: Delegate.t) => <BakerItem baker />;
};

module BakerInputTypeToogle = {
  let styles =
    Style.(
      StyleSheet.create({
        "inputTypeButton":
          style(~flexDirection=`row, ~alignItems=`center, ()),
        "inputTypeText": style(~lineHeight=16., ~marginLeft=7.->dp, ()),
      })
    );

  [@react.component]
  let make = (~inputType, ~onPress) => {
    let theme = ThemeContext.useTheme();
    <TouchableOpacity style=styles##inputTypeButton onPress>
      {switch (inputType) {
       | Selector =>
         <>
           <Icons.Edit size=14. color={theme.colors.iconPrimary} />
           <Typography.ButtonSecondary
             style=Style.(
               array([|
                 styles##inputTypeText,
                 style(~color=theme.colors.textPrimary, ()),
               |])
             )>
             "CUSTOM"->React.string
           </Typography.ButtonSecondary>
         </>
       | Text =>
         <>
           <Icons.List size=14. color={theme.colors.iconPrimary} />
           <Typography.ButtonSecondary
             style=Style.(
               array([|
                 styles##inputTypeText,
                 style(~color=theme.colors.textPrimary, ()),
               |])
             )>
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
      "formGroup": style(~zIndex=10, ~marginBottom=0.->dp, ()),
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

  let items = bakersRequest->ApiRequest.getDoneOk->Option.getWithDefault([||]);

  let (inputType, setInputType) = React.useState(_ => Selector);

  React.useEffect4(
    () => {
      if (inputType == Selector) {
        if (value == "") {
          // if input selector and no value, select first entry
          let firstItem = items->Array.get(0);
          firstItem->Lib.Option.iter(baker => baker.address->handleChange);
        } else if (items->Array.size > 0
                   && !items->Array.some(baker => baker.address == value)) {
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
           getItemValue={baker => baker.address}
           onValueChange=handleChange
           selectedValue=value
           hasError
           renderButton=BakerSelector.renderButton
           renderItem=BakerSelector.renderItem
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
    <FormError ?error />
  </FormGroup>;
};
