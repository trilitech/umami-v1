open ReactNative;
open UmamiCommon;

module SecretItem = {
  let itemStyles =
    Style.(
      StyleSheet.create({
        "inner": style(~height=44.->dp, ~justifyContent=`spaceBetween, ()),
        "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
      })
    );

  [@react.component]
  let make = (~style as paramStyle=?, ~secret: Secret.t) => {
    <View style=Style.(arrayOption([|Some(itemStyles##inner), paramStyle|]))>
      <View style=itemStyles##info>
        <Typography.Subtitle2>
          secret.name->React.string
        </Typography.Subtitle2>
      </View>
      <Typography.Address>
        secret.derivationScheme->React.string
      </Typography.Address>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "label": style(~marginBottom=6.->dp, ()),
      "itemInSelector": style(~marginHorizontal=20.->dp, ()),
      "selectorContent":
        style(
          ~height=66.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "spacer": style(~height=6.->dp, ()),
    })
  );

let renderButton = (selectedSecret: option(Secret.t), _hasError) =>
  <View style=styles##selectorContent>
    {selectedSecret->Option.mapWithDefault(<LoadingView />, secret =>
       <SecretItem style=styles##itemInSelector secret />
     )}
  </View>;

let renderItem = (secret: Secret.t) =>
  <SecretItem style=styles##itemInSelector secret />;

[@react.component]
let make = (~label, ~value: option(string), ~handleChange, ~error, ~disabled) => {
  let secretsRequest = StoreContext.Secrets.useLoad();

  let hasError = error->Option.isSome;

  let items =
    secretsRequest
    ->ApiRequest.getWithDefault([||])
    ->SortArray.stableSortBy((a, b) =>
        Js.String.localeCompare(b.name, a.name)->int_of_float
      );

  React.useEffect2(
    () => {
      if (value == None) {
        let firstItem = items->Array.get(0);
        firstItem->Lib.Option.iter(secret => secret->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        getItemKey={secret => secret.index->string_of_int}
        onValueChange=handleChange
        selectedValueKey=?value
        renderButton
        renderItem
        disabled
        keyPopover="formGroupSecretSelector"
      />
    </View>
  </FormGroup>;
};
