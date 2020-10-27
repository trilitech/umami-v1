open ReactNative;

type item = {
  value: string,
  label: string,
};

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~height=58.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~item, ~onChange, ~renderItem) => {
    <TouchableOpacity onPress={_e => onChange(item.value)}>
      <View style=styles##itemContainer> {renderItem(item)} </View>
    </TouchableOpacity>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "listContainer":
        style(
          ~position=`absolute,
          ~top=0.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
          ~paddingVertical=8.->dp,
          ~backgroundColor="#2e2e2e",
          ~borderRadius=3.,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~style=?,
      ~items: array(item),
      ~selectedValue=?,
      ~onValueChange,
      ~renderButton,
      ~renderItem,
    ) => {
  let (isOpen, setIsOpen) = React.useState(_ => false);

  let onChange = newValue => {
    onValueChange(newValue);
    setIsOpen(_ => false);
  };

  let selectedItem =
    items->Belt.Array.getBy(item =>
      item.value == selectedValue->Belt.Option.getWithDefault("")
    );

  <View ?style>
    <TouchableOpacity onPress={_e => setIsOpen(prevIsOpen => !prevIsOpen)}>
      {renderButton(selectedItem)}
    </TouchableOpacity>
    {isOpen
       ? <View>
           <View style=styles##listContainer>
             {items
              ->Belt.Array.map(item =>
                  <Item key={item.value} item onChange renderItem />
                )
              ->React.array}
           </View>
         </View>
       : React.null}
  </View>;
};
