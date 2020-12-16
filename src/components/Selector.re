open ReactNative;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~paddingVertical=5.->dp,
            ~paddingRight=(24. +. 20.)->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~item, ~onChange, ~renderItem) => {
    <ThemedPressable
      onPress={_e => onChange(item)} style=styles##itemContainer>
      {renderItem(item)}
    </ThemedPressable>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
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
          ~borderRadius=3.,
          (),
        ),
      "listContentContainer": style(~paddingVertical=8.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~style=?,
      ~items: array('item),
      ~getItemValue: 'item => string,
      ~selectedValue=?,
      ~onValueChange,
      ~noneItem: option('item)=?,
      ~renderButton,
      ~renderItem,
      ~disabled=false,
    ) => {
  let touchableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);

  DocumentContext.useClickOutside(
    touchableRef,
    isOpen,
    React.useCallback1(_pressEvent => setIsOpen(_ => false), [|setIsOpen|]),
  );

  let onChange = newItem => {
    onValueChange(newItem->getItemValue);
  };

  let selectedItem =
    items->Belt.Array.getBy(item =>
      item->getItemValue == selectedValue->Belt.Option.getWithDefault("")
    );

  let theme = ThemeContext.useTheme();

  <View ?style>
    <ThemedPressable
      pressableRef={touchableRef->Ref.value}
      style=Style.(
        array([|
          styles##button,
          style(~borderColor=theme.colors.borderMediumEmphasis, ()),
        |])
      )
      onPress={_e => setIsOpen(prevIsOpen => !prevIsOpen)}
      disabled>
      {renderButton(
         selectedItem->Belt.Option.isSome ? selectedItem : noneItem,
       )}
      {disabled
         ? React.null
         : <Icons.ChevronDown
             size=24.
             color={theme.colors.iconMediumEmphasis}
             style=styles##icon
           />}
    </ThemedPressable>
    <View style={ReactUtils.displayOn(isOpen)}>
      <ScrollView
        style=Style.(
          array([|
            styles##listContainer,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )
        contentContainerStyle=Style.(
          array([|
            styles##listContentContainer,
            style(~backgroundColor=theme.colors.stateActive, ()),
          |])
        )>
        {noneItem->Belt.Option.mapWithDefault(React.null, item =>
           <Item key={item->getItemValue} item onChange renderItem />
         )}
        {items
         ->Belt.Array.map(item =>
             <Item key={item->getItemValue} item onChange renderItem />
           )
         ->React.array}
      </ScrollView>
    </View>
  </View>;
};
