open ReactNative;

type colorStyle = [ | `highEmphasis | `mediumEmphasis | `disabled];

let getColor = colorStyle =>
  switch (colorStyle) {
  | `highEmphasis => Theme.colorDarkHighEmphasis
  | `mediumEmphasis => Theme.colorDarkMediummphasis
  | `disabled => Theme.colorDarkDisabled
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

module Make = (DefaultStyle: TextDesignStyle) => {
  let styles =
    Style.(StyleSheet.create({"text": style(~fontFamily="Avenir", ())}));

  [@react.component]
  let make =
      (
        ~colorStyle=DefaultStyle.colorStyle,
        ~fontSize=DefaultStyle.fontSize,
        ~fontWeightStyle=DefaultStyle.fontWeightStyle,
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

module Default =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `book;
    let fontSize = 14.;
  });

/* OVERLINE */

module Overline1 =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 18.;
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

module Subtitle2 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 16.;
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

/* BUTTON */

module ButtonPrimary12 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 12.;
  });

module ButtonPrimary10 =
  Make({
    let colorStyle = `highEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 10.;
  });

module ButtonSecondary =
  Make({
    let colorStyle = `mediumEmphasis;
    let fontWeightStyle = `heavy;
    let fontSize = 12.;
  });
