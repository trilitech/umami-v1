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

type focusOutlineColor =
  | Default
  | Color(string);

type focusOutlineConfig = (focusOutlineColor, float);

let styles =
  Style.(StyleSheet.create({"container": style(~overflow=`hidden, ())}));

[@react.component]
let make =
    (
      ~pressableRef=?,
      ~onPress=?,
      ~href=?,
      ~style as styleFromProp=?,
      ~outerStyle=?,
      ~tooltip=?,
      ~interactionStyle:
         option(Pressable_.interactionState => option(ReactNative.Style.t))=?,
      ~isActive=false,
      ~disabled=false,
      ~isPrimary=false,
      ~focusOutline: option(focusOutlineConfig)=?,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  let (pressableRef, isOpen, popoverConfig, togglePopover, setClosed) =
    Popover.usePopoverState(~elementRef=?pressableRef, ());

  let backgroundColor = (~pressed, ~hovered, ~focused) =>
    if (disabled) {
      isPrimary ? Some(theme.colors.primaryStateDisabled) : None;
    } else if (pressed) {
      isPrimary
        ? Some(theme.colors.primaryStatePressed)
        : Some(theme.colors.statePressed);
    } else if (hovered || focused && focusOutline->Option.isNone) {
      isPrimary
        ? Some(theme.colors.primaryStateHovered)
        : Some(theme.colors.stateHovered);
    } else {
      None;
    };

  let outlineColor = (~focused) =>
    switch (focused, focusOutline) {
    | (true, Some((Default, size))) =>
      Some((theme.colors.stateFocusedOutline, size))
    | (true, Some((Color(color), size))) => Some((color, size))
    | _ => None
    };

  <View
    style=?outerStyle
    onMouseEnter={_ => togglePopover()}
    onMouseLeave={_ => setClosed()}>
    <Pressable_
      ref={pressableRef->Ref.value} ?onPress disabled ?href ?accessibilityRole>
      {({hovered, pressed, focused} as interactionState) => {
         let hovered = hovered->Option.getWithDefault(false);
         let focused = focused->Option.getWithDefault(false);
         <View
           pointerEvents=`none
           style=Style.(
             arrayOption([|
               Some(styles##container),
               styleFromProp,
               backgroundColor(~pressed, ~hovered, ~focused)
               ->Option.map(b => Style.(style(~backgroundColor=b, ()))),
               outlineColor(~focused)
               ->Option.map(((c, s)) =>
                   Style.(
                     style()
                     ->unsafeAddStyle({
                         "boxShadow": {j|0px 0px 0px $(s)px $c|j},
                       })
                   )
                 ),
               interactionStyle->Option.flatMap(interactionStyle =>
                 interactionStyle(interactionState)
               ),
             |])
           )>
           {ReactUtils.mapOpt(tooltip, ((keyPopover, text)) => {
              <Tooltip keyPopover text isOpen config=popoverConfig />
            })}
           <View
             style=Style.(
               arrayOption([|
                 Some(StyleSheet.absoluteFillObject),
                 isActive
                   ? Some(
                       Style.style(
                         ~backgroundColor=
                           isPrimary
                             ? theme.colors.primaryStateActive
                             : theme.colors.stateActive,
                         (),
                       ),
                     )
                   : None,
               |])
             )
           />
           children
         </View>;
       }}
    </Pressable_>
  </View>;
};
