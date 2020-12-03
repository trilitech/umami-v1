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
        "itemContainerHovered":
          style(~backgroundColor=Theme.colorDarkSelected, ()),
      })
    );

  [@react.component]
  let make = (~item, ~onChange, ~renderItem) => {
    <PressableCustom onPress={_e => onChange(item.value)}>
      {interactionState =>
         <View
           style=Style.(
             arrayOption([|
               Some(styles##itemContainer),
               interactionState.hovered
                 ? Some(styles##itemContainerHovered) : None,
             |])
           )>
           {renderItem(item)}
         </View>}
    </PressableCustom>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~borderColor="rgba(255,255,255,0.6)",
          ~borderWidth=1.,
          ~borderRadius=5.,
          (),
        ),
      "icon": style(~marginRight=20.->dp, ()),
      "listContainer":
        style(
          ~position=`absolute,
          ~top=0.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
          ~maxHeight=224.->dp,
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
      ~disabled=false,
    ) => {
  let touchableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);

  DocumentContext.useDocumentPress(
    React.useCallback1(
      pressEvent =>
        if (touchableRef.current !==
            pressEvent->Event.PressEvent.nativeEvent##target) {
          setIsOpen(_ => false);
        },
      [|setIsOpen|],
    ),
  );

  let onChange = newValue => {
    onValueChange(newValue);
  };

  let selectedItem =
    items->Belt.Array.getBy(item =>
      item.value == selectedValue->Belt.Option.getWithDefault("")
    );

  <View ?style>
    <TouchableOpacity
      ref={touchableRef->Ref.value}
      onPress={_e => setIsOpen(prevIsOpen => !prevIsOpen)}
      disabled>
      <View style=styles##button pointerEvents=`none>
        {renderButton(selectedItem)}
        {disabled
           ? React.null
           : <Icons.ChevronDown
               size=24.
               color=Theme.colorDarkMediumEmphasis
               style=styles##icon
             />}
      </View>
    </TouchableOpacity>
    <View style={ReactUtils.displayOn(isOpen)}>
      <ScrollView style=styles##listContainer>
        {items
         ->Belt.Array.map(item =>
             <Item key={item.value} item onChange renderItem />
           )
         ->React.array}
      </ScrollView>
    </View>
  </View>;
};
