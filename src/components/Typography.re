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

type fontWeightStyle = [ | `black | `heavy | `medium | `book | `light];

let getFontWeight = fontWeightStyle =>
  switch (fontWeightStyle) {
  | `black => `_900
  | `heavy => `bold
  | `medium => `_500
  | `book => `normal
  | `light => `_300
  };

module type TextDesignStyle = {
  let colorStyle: colorStyle;
  let fontWeightStyle: fontWeightStyle;
  let fontSize: float;
};

module Base = {
  let styles =
    Style.(StyleSheet.create({"text": style(~fontFamily="Avenir", ())}));

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
    let fontWeightStyle = `black;
    let fontSize = 24.;
  });

module Headline2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `black;
    let fontSize = 22.;
  });

/* OVERLINE */

module Overline1 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 16.;
  });

module Overline2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `light;
    let fontSize = 18.;
  });

module Overline3 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `light;
    let fontSize = 14.;
  });

/* SUBTITLE */

module Subtitle1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 14.;
  });

module Subtitle2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 16.;
  });

module Subtitle3 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `black;
    let fontSize = 14.;
  });

module Subtitle4 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 12.;
  });

/* BODY */

module Body1 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `medium;
    let fontSize = 16.;
  });

module Body2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `book;
    let fontSize = 16.;
  });

module Body3 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `medium;
    let fontSize = 14.;
  });

/* BUTTON */

module ButtonPrimary =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 14.;
  });

module ButtonSecondary =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 12.;
  });
