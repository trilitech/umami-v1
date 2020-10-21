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

/* OVERLINE */

module Overline1 = {
  [@react.component]
  let make = (~children, ~numberOfLines=?, ~style=?) => {
    <Base
      colorStyle=`mediumEmphasis
      fontWeightStyle=`heavy
      fontSize=18.
      ?numberOfLines
      ?style>
      children
    </Base>;
  };
};

module Overline2 = {
  [@react.component]
  let make = (~children, ~numberOfLines=?, ~style=?) => {
    <Base
      colorStyle=`highEmphasis
      fontWeightStyle=`light
      fontSize=18.
      ?numberOfLines
      ?style>
      children
    </Base>;
  };
};

module Overline3 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`mediumEmphasis) => {
    <Base colorStyle fontWeightStyle=`light fontSize=14. ?numberOfLines ?style>
      children
    </Base>;
  };
};

/* SUBTITLE */

module Subtitle2 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`highEmphasis) => {
    <Base colorStyle fontWeightStyle=`heavy fontSize=16. ?numberOfLines ?style>
      children
    </Base>;
  };
};

/* BODY */

module Body1 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`highEmphasis) => {
    <Base
      colorStyle fontWeightStyle=`medium fontSize=16. ?numberOfLines ?style>
      children
    </Base>;
  };
};

module Body2 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`highEmphasis) => {
    <Base colorStyle fontWeightStyle=`book fontSize=16. ?numberOfLines ?style>
      children
    </Base>;
  };
};

/* BUTTON */

module ButtonPrimary12 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`highEmphasis) => {
    <Base colorStyle fontWeightStyle=`heavy fontSize=12. ?numberOfLines ?style>
      children
    </Base>;
  };
};

module ButtonPrimary10 = {
  [@react.component]
  let make =
      (~children, ~numberOfLines=?, ~style=?, ~colorStyle=`highEmphasis) => {
    <Base colorStyle fontWeightStyle=`heavy fontSize=10. ?numberOfLines ?style>
      children
    </Base>;
  };
};

module ButtonSecondary = {
  [@react.component]
  let make = (~children, ~numberOfLines=?, ~style=?) => {
    <Base
      colorStyle=`mediumEmphasis
      fontWeightStyle=`heavy
      fontSize=12.
      ?numberOfLines
      ?style>
      children
    </Base>;
  };
};
