open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=10, ()),
      "label": style(~marginBottom=6.->dp, ()),
      "selectorContent":
        style(
          ~height=46.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "inner": style(~flex=1., ~flexDirection=`row, ~paddingLeft=20.->dp, ()),
      "address": style(~flexBasis=120.->dp, ~marginLeft=auto, ()),
    })
  );

let renderButton = (selectedItem: option(Selector.item)) =>
  <View style=styles##selectorContent>
    {selectedItem->Belt.Option.mapWithDefault(<LoadingView />, item =>
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

  let hasError = error->Belt.Option.isSome;

  let items =
    bakersRequest
    ->ApiRequest.getDoneOk
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.map(baker =>
        {Selector.value: baker.address, label: baker.name}
      );

  React.useEffect2(
    () => {
      if (value == "") {
        let firstItem = items->Belt.Array.get(0);
        firstItem->Common.Lib.Option.iter(item => item.value->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        onValueChange=handleChange
        selectedValue=value
        renderButton
        renderItem=AccountSelector.renderItem
      />
    </View>
  </FormGroup>;
};
