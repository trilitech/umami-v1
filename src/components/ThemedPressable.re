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

let styles =
  Style.(StyleSheet.create({"container": style(~overflow=`hidden, ())}));

module LayerActiveState = {
  [@react.component]
  let make = (~isActive=false, ~color) => {
    <View
      style=Style.(
        arrayOption([|
          Some(StyleSheet.absoluteFillObject),
          isActive ? Some(Style.style(~backgroundColor=color, ())) : None,
        |])
      )
    />;
  };
};

module ContainerInteractionState = {
  module type Config = {
    let backgroundColor:
      (
        ~disabled: bool,
        ~pressed: bool,
        ~hovered: bool,
        ~focused: bool,
        ThemeContext.theme
      ) =>
      option(string);
    let outlineColor:
      (~focused: bool, ~defaultColor: option(string), ThemeContext.theme) =>
      option(string);
    let activeColor: ThemeContext.theme => string;
    let defaultFocusedSize: float;
  };

  module Make = (Config: Config) => {
    [@react.component]
    let make =
        (
          ~hovered,
          ~pressed,
          ~focused,
          ~disabled=false,
          ~isActive=false,
          ~children,
          ~style as styleFromProp=?,
          ~focusedSize=Config.defaultFocusedSize,
          ~focusedColor=?,
        ) => {
      let theme = ThemeContext.useTheme();

      let backgroundColor =
        Config.backgroundColor(
          ~disabled,
          ~pressed,
          ~hovered,
          ~focused,
          theme,
        );

      let outlineColor =
        Config.outlineColor(~focused, ~defaultColor=focusedColor, theme);

      let activeColor = Config.activeColor(theme);

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
        <LayerActiveState isActive color=activeColor />
        children
      </View>;
    };
  };

  module BaseConfig: Config = {
    let backgroundColor =
        (~disabled, ~pressed, ~hovered, ~focused, theme: ThemeContext.theme) =>
      if (disabled) {
        None;
      } else if (pressed) {
        Some(theme.colors.statePressed);
      } else if (hovered || focused) {
        Some(theme.colors.stateHovered);
      } else {
        None;
      };
    let outlineColor = (~focused as _f, ~defaultColor as _c, _theme) => None;
    let activeColor = (theme: ThemeContext.theme) => theme.colors.stateActive;
    let defaultFocusedSize = 0.;
  };

  module Base = Make(BaseConfig);

  module type T = (module type of Base);

  module OutlineConfig: Config = {
    let backgroundColor =
        (
          ~disabled,
          ~pressed,
          ~hovered,
          ~focused as _f,
          theme: ThemeContext.theme,
        ) =>
      if (disabled) {
        None;
      } else if (pressed) {
        Some(theme.colors.statePressed);
      } else if (hovered) {
        Some(theme.colors.stateHovered);
      } else {
        None;
      };
    let outlineColor = (~focused, ~defaultColor, theme: ThemeContext.theme) =>
      switch (focused, defaultColor) {
      | (true, None) => Some(theme.colors.stateFocusedOutline)
      | (true, Some(color)) => Some(color)
      | _ => None
      };
    let activeColor = (theme: ThemeContext.theme) => theme.colors.stateActive;
    let defaultFocusedSize = 2.;
  };

  module Outline = Make(OutlineConfig);

  module PrimaryConfig: Config = {
    let backgroundColor =
        (
          ~disabled,
          ~pressed,
          ~hovered,
          ~focused as _f,
          theme: ThemeContext.theme,
        ) =>
      if (disabled) {
        Some(theme.colors.primaryStateDisabled);
      } else if (pressed) {
        Some(theme.colors.primaryStatePressed);
      } else if (hovered) {
        Some(theme.colors.primaryStateHovered);
      } else {
        None;
      };
    let outlineColor = (~focused, ~defaultColor, theme: ThemeContext.theme) =>
      switch (focused, defaultColor) {
      | (true, None) => Some(theme.colors.primaryButtonOutline)
      | (true, Some(color)) => Some(color)
      | _ => None
      };
    let activeColor = (theme: ThemeContext.theme) =>
      theme.colors.primaryStateActive;
    let defaultFocusedSize = 3.;
  };

  module Primary = Make(PrimaryConfig);
};

module Make = (ContainerInteractionState: ContainerInteractionState.T) => {
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
    <Pressable_
      ref=?{pressableRef->Option.map(Ref.value)}
      ?onPress
      disabled
      ?href
      ?accessibilityRole>
      {({hovered, pressed, focused}) => {
         let hovered = hovered->Option.getWithDefault(false);
         let focused = focused->Option.getWithDefault(false);
         <ContainerInteractionState
           disabled
           hovered
           pressed
           focused
           isActive
           style=?styleFromProp
           ?focusedSize
           ?focusedColor>
           children
         </ContainerInteractionState>;
       }}
    </Pressable_>;
  };
};

module Base = Make(ContainerInteractionState.Base);

include Base;

module type T = (module type of Base);

module Outline = Make(ContainerInteractionState.Outline);
module Primary = Make(ContainerInteractionState.Primary);
