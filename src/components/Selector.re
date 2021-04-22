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
      isActive=isSelected
      accessibilityRole=`button>
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
          ~padding=1.->dp,
          ~borderRadius=5.,
          (),
        ),
      "icon": style(~marginHorizontal=8.->dp, ()),
      "iconSpacer": style(~width=(8. +. 24. +. 8.)->dp, ()),
      "dropdownmenu": style(~maxHeight=224.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~style=?,
      ~dropdownStyle=?,
      ~items: array('item),
      ~selectedValueKey: option(string)=?,
      ~onValueChange: 'item => unit,
      ~getItemKey: 'item => string,
      ~noneItem: option('item)=?,
      ~renderButton,
      ~renderItem,
      ~hasError=false,
      ~disabled=false,
      ~keyPopover,
    ) => {
  let disabled = disabled || items->Array.size == 1 && noneItem->Option.isNone;

  let (pressableRef, isOpen, popoverConfig, togglePopover, _) =
    Popover.usePopoverState();

  let (_, animatedOpenValue) =
    AnimationHooks.useAnimationOpen(~speed=80., ~bounciness=0., isOpen, _ =>
      ()
    );

  let onChange = newItem => {
    onValueChange(newItem);
  };

  let isSelected = item =>
    selectedValueKey->Option.mapWithDefault(false, sel =>
      sel == item->getItemKey
    );

  let selectedItem = items->Array.getBy(isSelected);

  let theme = ThemeContext.useTheme();

  let borderColor =
    isOpen
      ? theme.colors.borderPrimary
      : hasError ? theme.colors.error : theme.colors.borderMediumEmphasis;

  <View ?style>
    <Pressable_
      ref={pressableRef->Ref.value} onPress={_ => togglePopover()} disabled>
      {_ =>
         <View
           style=Style.(
             array([|
               styles##button,
               style(~backgroundColor=theme.colors.background, ()),
               style(~borderColor, ~borderWidth=2., ~padding=0.->dp, ()),
             |])
           )
           pointerEvents=`none>
           {renderButton(
              selectedItem->Option.isSome ? selectedItem : noneItem,
              hasError,
            )}
           {disabled
              ? <View style=styles##iconSpacer />
              : <Animated.View
                  style=Style.(
                    style(
                      ~transform=[|
                        rotate(
                          ~rotate=
                            Animated.Interpolation.(
                              animatedOpenValue->interpolate(
                                config(
                                  ~inputRange=[|0., 1.|],
                                  ~outputRange=
                                    [|"0deg", "180deg"|]->fromStringArray,
                                  ~extrapolate=`clamp,
                                  (),
                                ),
                              )
                            )
                            ->Animated.StyleProp.angle,
                        ),
                      |],
                      (),
                    )
                  )>
                  <Icons.ChevronDown
                    size=24.
                    color={theme.colors.iconMediumEmphasis}
                    style=styles##icon
                  />
                </Animated.View>}
         </View>}
    </Pressable_>
    <DropdownMenu
      keyPopover
      style={Style.arrayOption([|
        Some(styles##dropdownmenu),
        dropdownStyle,
      |])}
      isOpen
      popoverConfig
      onRequestClose=togglePopover>
      {noneItem->Option.mapWithDefault(React.null, item =>
         <Item
           key={item->getItemKey}
           item
           onChange
           renderItem
           isSelected={selectedValueKey->Option.isNone}
         />
       )}
      {items
       ->Array.map(item =>
           <Item
             key={item->getItemKey}
             item
             onChange
             renderItem
             isSelected={item->isSelected}
           />
         )
       ->React.array}
    </DropdownMenu>
  </View>;
};
