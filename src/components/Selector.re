/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

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

  let (borderColor, borderWidth, padding) =
    isOpen
      ? (theme.colors.borderPrimary, Some(2.), Some(0.->Style.dp))
      : hasError
          ? (theme.colors.error, Some(2.), Some(0.->Style.dp))
          : (theme.colors.borderMediumEmphasis, None, None);

  <View ?style>
    <Pressable_
      ref={pressableRef->Ref.value} onPress={_ => togglePopover()} disabled>
      {_ =>
         <View
           style=Style.(
             array([|
               styles##button,
               style(~backgroundColor=theme.colors.background, ()),
               style(~borderColor, ~borderWidth?, ~padding?, ()),
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
      {_ =>
         [|
           {
             noneItem->Option.mapWithDefault(React.null, item =>
               <Item
                 key={item->getItemKey}
                 item
                 onChange
                 renderItem
                 isSelected={selectedValueKey->Option.isNone}
               />
             );
           },
         |]
         ->Array.concat(
             {
               items->Array.map(item =>
                 <Item
                   key={item->getItemKey}
                   item
                   onChange
                   renderItem
                   isSelected={item->isSelected}
                 />
               );
             },
           )}
    </DropdownMenu>
  </View>;
};
