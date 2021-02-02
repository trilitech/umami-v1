open ReactNative;

type colorStyle = [
  | `highEmphasis
  | `mediumEmphasis
  | `disabled
  | `error
  | `positive
  | `negative
  | `primaryHighEmphasis
  | `primaryMediumEmphasis
  | `primaryDisabled
];

let getColor = (colorStyle, theme: ThemeContext.theme) =>
  switch (colorStyle) {
  | `highEmphasis => theme.colors.textHighEmphasis
  | `mediumEmphasis => theme.colors.textMediumEmphasis
  | `disabled => theme.colors.textDisabled
  | `error => theme.colors.error
  | `positive => theme.colors.textPositive
  | `negative => theme.colors.textNegative
  | `primaryHighEmphasis => theme.colors.primaryTextHighEmphasis
  | `primaryMediumEmphasis => theme.colors.primaryTextMediumEmphasis
  | `primaryDisabled => theme.colors.primaryTextDisabled
  };

type fontWeightStyle = [
  | `black
  | `extraBold
  | `bold
  | `semiBold
  | `medium
  | `regular
  | `light
  | `extraLight
];

let getFontWeight = fontWeightStyle =>
  switch (fontWeightStyle) {
  | `black => `_900
  | `extraBold => `_800
  | `bold => `bold
  | `semiBold => `_600
  | `medium => `_500
  | `regular => `normal
  | `light => `_300
  | `extraLight => `_200
  };

module type TextDesignStyle = {
  let colorStyle: colorStyle;
  let fontWeightStyle: fontWeightStyle;
  let fontSize: float;
};

module Base = {
  let styles =
    Style.(
      StyleSheet.create({"text": style(~fontFamily="SourceSansPro", ())})
    );

  [@react.component]
  let make =
      (
        ~colorStyle: colorStyle,
        ~fontSize: float,
        ~fontWeightStyle: fontWeightStyle,
        ~numberOfLines: option(int)=?,
        ~style as styleProp: option(ReactNative.Style.t)=?,
        ~ellipsizeMode: option([ | `clip | `head | `middle | `tail])=?,
        ~children: React.element,
      ) => {
    let theme = ThemeContext.useTheme();
    <Text
      ?ellipsizeMode
      ?numberOfLines
      style=Style.(
        arrayOption([|
          Some(styles##text),
          Some(
            style(
              ~color=colorStyle->getColor(theme),
              ~fontSize,
              ~fontWeight=fontWeightStyle->getFontWeight,
              (),
            ),
          ),
          styleProp,
        |])
      )>
      children
    </Text>;
  };
};

module Make = (DefaultStyle: TextDesignStyle) => {
  let makeProps =
      (
        ~colorStyle=DefaultStyle.colorStyle,
        ~fontSize=DefaultStyle.fontSize,
        ~fontWeightStyle=DefaultStyle.fontWeightStyle,
        ~numberOfLines=?,
        ~ellipsizeMode=?,
        ~style=?,
        ~children,
      ) =>
    Base.makeProps(
      ~colorStyle,
      ~fontSize,
      ~fontWeightStyle,
      ~numberOfLines?,
      ~ellipsizeMode?,
      ~style?,
      ~children,
    );
  let make = Base.make;
};

/* H */

module Headline =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 22.;
  });

/* OVERLINE */

module Overline1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `light;
    let fontSize = 19.;
  });

module Overline2 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `semiBold;
    let fontSize = 16.;
  });

module Overline3 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `regular;
    let fontSize = 16.;
  });

/* SUBTITLE */

module Subtitle1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `semiBold;
    let fontSize = 16.;
  });

module Subtitle2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 15.;
  });

/* BODY */

module Body1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `regular;
    let fontSize = 16.;
  });

module Body2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `regular;
    let fontSize = 14.;
  });

/* BUTTON */

module ButtonPrimary =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 14.;
  });

module ButtonSecondary =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 12.;
  });

module ButtonTernary =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `semiBold;
    let fontSize = 11.;
  });

/* ADDRESS */

module Address = {
  let styles =
    Style.(
      StyleSheet.create({"address": style(~fontFamily="JetBrainsMono", ())})
    );

  [@react.component]
  let make =
      (
        ~numberOfLines: option(int)=?,
        ~style as styleProp: option(ReactNative.Style.t)=?,
        ~children,
      ) => {
    <Base
      colorStyle=`highEmphasis
      fontSize=14.
      fontWeightStyle=`extraLight
      style=Style.(arrayOption([|Some(styles##address), styleProp|]))
      ?numberOfLines>
      children
    </Base>;
  };
};
