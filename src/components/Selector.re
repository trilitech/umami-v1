open ReactNative;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~paddingVertical=5.->dp,
            ~paddingRight=40.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~item, ~onChange, ~renderItem, ~isSelected=false) => {
    <ThemedPressable
      onPress={_e => onChange(item)}
      style=styles##itemContainer
      isActive=isSelected>
      {renderItem(item)}
    </ThemedPressable>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "pressable": style(~zIndex=2, ()),
      "button":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~borderWidth=1.,
          ~padding=1.->dp,
          ~borderRadius=5.,
          (),
        ),
      "icon": style(~marginHorizontal=8.->dp, ()),
      "dropdownmenu":
        style(
          ~zIndex=1,
          ~position=`absolute,
          ~top=3.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
          ~maxHeight=224.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~style=?,
      ~dropdownStyle=?,
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
    items->Array.getBy(item =>
      item->getItemValue == selectedValue->Option.getWithDefault("")
    );

  let theme = ThemeContext.useTheme();

  <View ?style>
    <PressableCustom
      ref={touchableRef->Ref.value}
      style=styles##pressable
      onPress={_e => setIsOpen(prevIsOpen => !prevIsOpen)}
      disabled>
      {_ =>
         <View
           style=Style.(
             arrayOption([|
               Some(styles##button),
               Some(
                 style(
                   ~borderColor=theme.colors.borderMediumEmphasis,
                   ~backgroundColor=theme.colors.background,
                   (),
                 ),
               ),
               isOpen
                 ? Some(
                     style(
                       ~borderColor=theme.colors.borderPrimary,
                       ~borderWidth=2.,
                       ~padding=0.->dp,
                       (),
                     ),
                   )
                 : None,
             |])
           )
           pointerEvents=`none>
           {renderButton(
              selectedItem->Option.isSome ? selectedItem : noneItem,
            )}
           {disabled
              ? React.null
              : <Icons.ChevronDown
                  size=24.
                  color={theme.colors.iconMediumEmphasis}
                  style=styles##icon
                />}
         </View>}
    </PressableCustom>
    <DropdownMenu
      style={Style.arrayOption([|
        Some(styles##dropdownmenu),
        dropdownStyle,
      |])}
      isOpen>
      {noneItem->Option.mapWithDefault(React.null, item =>
         <Item
           key={item->getItemValue}
           item
           onChange
           renderItem
           isSelected={selectedValue->Option.isNone}
         />
       )}
      {items
       ->Array.map(item =>
           <Item
             key={item->getItemValue}
             item
             onChange
             renderItem
             isSelected={
               item->getItemValue == selectedValue->Option.getWithDefault("")
             }
           />
         )
       ->React.array}
    </DropdownMenu>
  </View>;
};
