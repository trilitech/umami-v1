open ReactNative;

type colorStyle = [ | `highEmphasis | `mediumEmphasis | `disabled | `error];

let getColor = colorStyle =>
  switch (colorStyle) {
  | `highEmphasis => Theme.colorDarkHighEmphasis
  | `mediumEmphasis => Theme.colorDarkMediumEmphasis
  | `disabled => Theme.colorDarkDisabled
  | `error => Theme.colorDarkError
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
        ~colorStyle,
        ~fontSize,
        ~fontWeightStyle,
        ~numberOfLines=?,
        ~style as styleProp=?,
        ~children,
      ) => {
    <Text
      ?numberOfLines
      style=Style.(
        arrayOption([|
          Some(styles##text),
          Some(
            style(
              ~color=colorStyle->getColor,
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
  [@react.component]
  let make =
      (
        ~colorStyle=DefaultStyle.colorStyle,
        ~fontSize=DefaultStyle.fontSize,
        ~fontWeightStyle=DefaultStyle.fontWeightStyle,
        ~numberOfLines=?,
        ~style=?,
        ~children,
      ) => {
    <Base colorStyle fontSize fontWeightStyle ?numberOfLines ?style>
      children
    </Base>;
  };
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
