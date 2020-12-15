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
        "itemContainerHovered":
          style(~backgroundColor=Theme.colorDarkSelected, ()),
      })
    );

  [@react.component]
  let make = (~item, ~onChange, ~renderItem) => {
    let theme = ThemeContext.useTheme();
    <PressableCustom onPress={_e => onChange(item)}>
      {({hovered, pressed}) =>
         <View
           style=Style.(
             arrayOption([|
               Some(styles##itemContainer),
               hovered
                 ? Some(
                     Style.style(
                       ~backgroundColor=theme.colors.stateHovered,
                       (),
                     ),
                   )
                 : None,
               pressed
                 ? Some(
                     Style.style(
                       ~backgroundColor=theme.colors.statePressed,
                       (),
                     ),
                   )
                 : None,
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
          //~paddingVertical=8.->dp,
          //~backgroundColor="#2e2e2e",
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

  let onChange = newItem => {
    onValueChange(newItem->getItemValue);
  };

  let selectedItem =
    items->Belt.Array.getBy(item =>
      item->getItemValue == selectedValue->Belt.Option.getWithDefault("")
    );

  let theme = ThemeContext.useTheme();

  <View ?style>
    <TouchableOpacity
      ref={touchableRef->Ref.value}
      onPress={_e => setIsOpen(prevIsOpen => !prevIsOpen)}
      disabled>
      <View
        style=Style.(
          array([|
            styles##button,
            style(~borderColor=theme.colors.borderMediumEmphasis, ()),
          |])
        )
        pointerEvents=`none>
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
      </View>
    </TouchableOpacity>
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
