open ReactNative;

type colorStyle = [
  | `highEmphasis
  | `mediumEmphasis
  | `mediumEmphasisOpposite
  | `disabled
  | `error
  | `positive
  | `negative
];

let getColor = (colorStyle, theme: ThemeContext.theme) =>
  switch (colorStyle) {
  | `highEmphasis => theme.colors.textHighEmphasis
  | `mediumEmphasis => theme.colors.textMediumEmphasis
  | `mediumEmphasisOpposite => theme.colors.primaryTextMediumEmphasis
  | `disabled => theme.colors.textDisabled
  | `error => theme.colors.error
  | `positive => theme.colors.textPositive
  | `negative => theme.colors.textNegative
  };

type fontWeightStyle = [
  | `extraBold
  | `bold
  | `semiBold
  | `medium
  | `regular
];

let getFontWeight = fontWeightStyle =>
  switch (fontWeightStyle) {
  | `extraBold => `_800
  | `bold => `bold
  | `semiBold => `_600
  | `medium => `_500
  | `regular => `normal
  };

module type TextDesignStyle = {
  let colorStyle: colorStyle;
  let fontWeightStyle: fontWeightStyle;
  let fontSize: float;
};

module Base = {
  let styles =
    Style.(StyleSheet.create({"text": style(~fontFamily="EBGaramond", ())}));

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

module Headline1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `extraBold;
    let fontSize = 24.;
  });

module Headline2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `extraBold;
    let fontSize = 22.;
  });

/* OVERLINE */

module Overline1 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 16.;
  });

module Overline2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `regular;
    let fontSize = 18.;
  });

module Overline3 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `regular;
    let fontSize = 14.;
  });

/* SUBTITLE */

module Subtitle1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 14.;
  });

module Subtitle2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 16.;
  });

module Subtitle3 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `extraBold;
    let fontSize = 14.;
  });

module Subtitle4 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `bold;
    let fontSize = 12.;
  });

/* BODY */

module Body1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `semiBold;
    let fontSize = 16.;
  });

module Body2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `medium;
    let fontSize = 16.;
  });

module Body3 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `semiBold;
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

/* ADDRESS */

module Address = {
  let styles =
    Style.(
      StyleSheet.create({"address": style(~fontFamily="CutiveMono", ())})
    );

  [@react.component]
  let make =
      (
        ~fontSize=14.,
        ~numberOfLines: option(int)=?,
        ~style as styleProp: option(ReactNative.Style.t)=?,
        ~children,
      ) => {
    <Base
      colorStyle=`highEmphasis
      fontSize
      fontWeightStyle=`regular
      style=Style.(arrayOption([|Some(styles##address), styleProp|]))
      ?numberOfLines>
      children
    </Base>;
  };
};
