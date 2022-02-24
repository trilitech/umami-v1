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
        "button":
          style(
            ~height=30.->dp,
            ~paddingHorizontal=14.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
        "text": style(~marginLeft=10.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~text,
        ~icon: Icons.builder,
        ~colorStyle=`highEmphasis,
        ~onPress=?,
        ~disabled=?,
      ) => {
    let theme = ThemeContext.useTheme();
    <ThemedPressable
      ?onPress style=styles##button accessibilityRole=`button ?disabled>
      {icon(
         ~style=?None,
         ~size=20.,
         ~color=
           (colorStyle === `highEmphasis ? `mediumEmphasis : colorStyle)
           ->Typography.getColor(theme),
       )}
      <Typography.ButtonSecondary colorStyle fontSize=14. style=styles##text>
        text->React.string
      </Typography.ButtonSecondary>
    </ThemedPressable>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "dropdownmenu":
        style(
          ~top=2.->dp,
          ~right=2.->dp,
          ~minWidth=170.->dp,
          ~maxHeight=224.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~keyPopover,
      ~icon: Icons.builder,
      ~children,
      ~size=34.,
      ~iconSizeRatio=?,
      ~style as styleArg=?,
    ) => {
  let (pressableRef, isOpen, popoverConfig, togglePopover, _) =
    Popover.usePopoverState();

  <View
    style=Style.(
      arrayOption([|
        Some(style(~width=size->dp, ~height=size->dp, ())),
        styleArg,
      |])
    )>
    <IconButton
      pressableRef
      isActive=isOpen
      icon
      size
      ?iconSizeRatio
      onPress={_ => togglePopover()}
    />
    <DropdownMenu
      keyPopover
      openingStyle=Popover.TopRight
      style=styles##dropdownmenu
      isOpen
      popoverConfig
      onRequestClose=togglePopover>
      {_ => children}
    </DropdownMenu>
  </View>;
};
