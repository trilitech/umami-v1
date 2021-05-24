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

module ContainerInteractionState = {
  module Base = {
    [@react.component]
    let make =
        (
          ~hovered,
          ~pressed,
          ~focused,
          ~disabled,
          ~children,
          ~style as styleFromProp=?,
        ) => {
      let theme = ThemeContext.useTheme();

      let backgroundColor =
        if (disabled) {
          None;
        } else if (pressed) {
          Some(theme.colors.statePressed);
        } else if (hovered || focused) {
          Some(theme.colors.stateHovered);
        } else {
          None;
        };

      <View
        pointerEvents=`none
        style=Style.(
          arrayOption([|
            Some(styles##container),
            styleFromProp,
            backgroundColor->Option.map(b =>
              Style.(style(~backgroundColor=b, ()))
            ),
          |])
        )>
        children
      </View>;
    };
  };

  module Outline = {
    [@react.component]
    let make =
        (
          ~hovered,
          ~pressed,
          ~focused,
          ~disabled,
          ~children,
          ~style as styleFromProp=?,
          ~focusedSize=2.,
          ~focusedColor=?,
        ) => {
      let theme = ThemeContext.useTheme();

      let backgroundColor =
        if (disabled) {
          None;
        } else if (pressed) {
          Some(theme.colors.statePressed);
        } else if (hovered) {
          Some(theme.colors.stateHovered);
        } else {
          None;
        };

      let outlineColor =
        switch (focused, focusedColor) {
        | (true, None) => Some(theme.colors.stateFocusedOutline)
        | (true, Some(color)) => Some(color)
        | _ => None
        };

      <View
        pointerEvents=`none
        style=Style.(
          arrayOption([|
            Some(styles##container),
            styleFromProp,
            backgroundColor->Option.map(b =>
              Style.(style(~backgroundColor=b, ()))
            ),
            outlineColor->Option.map(c =>
              Style.(
                style()
                ->unsafeAddStyle({
                    "boxShadow": {j|0px 0px 0px $(focusedSize)px $c|j},
                  })
              )
            ),
          |])
        )>
        children
      </View>;
    };
  };

  module Primary = {
    [@react.component]
    let make =
        (
          ~hovered,
          ~pressed,
          ~focused,
          ~disabled,
          ~children,
          ~style as styleFromProp=?,
          ~focusedSize=3.,
          ~focusedColor=?,
        ) => {
      let theme = ThemeContext.useTheme();

      let backgroundColor =
        if (disabled) {
          Some(theme.colors.primaryStateDisabled);
        } else if (pressed) {
          Some(theme.colors.primaryStatePressed);
        } else if (hovered) {
          Some(theme.colors.primaryStateHovered);
        } else {
          None;
        };

      let outlineColor =
        switch (focused, focusedColor) {
        | (true, None) => Some(theme.colors.primaryButtonOutline)
        | (true, Some(color)) => Some(color)
        | _ => None
        };

      <View
        pointerEvents=`none
        style=Style.(
          arrayOption([|
            Some(styles##container),
            styleFromProp,
            backgroundColor->Option.map(b =>
              Style.(style(~backgroundColor=b, ()))
            ),
            outlineColor->Option.map(c =>
              Style.(
                style()
                ->unsafeAddStyle({
                    "boxShadow": {j|0px 0px 0px $(focusedSize)px $c|j},
                  })
              )
            ),
          |])
        )>
        children
      </View>;
    };
  };
};

[@react.component]
let make =
    (
      ~pressableRef=?,
      ~onPress=?,
      ~href=?,
      ~style as styleFromProp=?,
      ~isActive=false,
      ~disabled=false,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  <Pressable_
    ref=?{pressableRef->Option.map(Ref.value)}
    ?onPress
    disabled
    ?href
    ?accessibilityRole>
    {({hovered, pressed, focused}) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <ContainerInteractionState.Base
         disabled hovered pressed focused style=?styleFromProp>
         <View
           style=Style.(
             arrayOption([|
               Some(StyleSheet.absoluteFillObject),
               isActive
                 ? Some(
                     Style.style(
                       ~backgroundColor=theme.colors.stateActive,
                       (),
                     ),
                   )
                 : None,
             |])
           )
         />
         children
       </ContainerInteractionState.Base>;
     }}
  </Pressable_>;
};

module Outline = {
  [@react.component]
  let make =
      (
        ~pressableRef=?,
        ~onPress=?,
        ~href=?,
        ~style as styleFromProp=?,
        ~focusedSize=?,
        ~focusedColor=?,
        ~isActive=false,
        ~disabled=false,
        ~accessibilityRole: option(Accessibility.role)=?,
        ~children,
      ) => {
    let theme = ThemeContext.useTheme();

    <Pressable_
      ref=?{pressableRef->Option.map(Ref.value)}
      ?onPress
      disabled
      ?href
      ?accessibilityRole>
      {({hovered, pressed, focused}) => {
         let hovered = hovered->Option.getWithDefault(false);
         let focused = focused->Option.getWithDefault(false);
         <ContainerInteractionState.Outline
           disabled
           hovered
           pressed
           focused
           style=?styleFromProp
           ?focusedSize
           ?focusedColor>
           <View
             style=Style.(
               arrayOption([|
                 Some(StyleSheet.absoluteFillObject),
                 isActive
                   ? Some(
                       Style.style(
                         ~backgroundColor=theme.colors.stateActive,
                         (),
                       ),
                     )
                   : None,
               |])
             )
           />
           children
         </ContainerInteractionState.Outline>;
       }}
    </Pressable_>;
  };
};

module Primary = {
  [@react.component]
  let make =
      (
        ~pressableRef=?,
        ~onPress=?,
        ~href=?,
        ~style as styleFromProp=?,
        ~focusedSize=?,
        ~focusedColor=?,
        ~isActive=false,
        ~disabled=false,
        ~accessibilityRole: option(Accessibility.role)=?,
        ~children,
      ) => {
    let theme = ThemeContext.useTheme();

    <Pressable_
      ref=?{pressableRef->Option.map(Ref.value)}
      ?onPress
      disabled
      ?href
      ?accessibilityRole>
      {({hovered, pressed, focused}) => {
         let hovered = hovered->Option.getWithDefault(false);
         let focused = focused->Option.getWithDefault(false);
         <ContainerInteractionState.Primary
           disabled
           hovered
           pressed
           focused
           style=?styleFromProp
           ?focusedSize
           ?focusedColor>
           <View
             style=Style.(
               arrayOption([|
                 Some(StyleSheet.absoluteFillObject),
                 isActive
                   ? Some(
                       Style.style(
                         ~backgroundColor=theme.colors.primaryStateActive,
                         (),
                       ),
                     )
                   : None,
               |])
             )
           />
           children
         </ContainerInteractionState.Primary>;
       }}
    </Pressable_>;
  };
};
