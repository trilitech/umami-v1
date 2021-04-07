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
      ~getItemValue: 'item => string,
      ~selectedValue=?,
      ~onValueChange,
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
    onValueChange(newItem->getItemValue);
  };

  let selectedItem =
    items->Array.getBy(item =>
      item->getItemValue == selectedValue->Option.getWithDefault("")
    );

  let theme = ThemeContext.useTheme();

  <View ?style>
    <Pressable_
      ref={pressableRef->Ref.value} onPress={_ => togglePopover()} disabled>
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
               hasError
                 ? Some(
                     style(
                       ~borderColor=theme.colors.error,
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
