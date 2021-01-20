type springConfig = {
  delay: float,
  onUpdate: float => unit,
  onComplete: unit => unit,
};

[@bs.module "react-flip-toolkit"]
external spring: springConfig => unit = "spring";

module Flipper = {
  [@react.component] [@bs.module "react-flip-toolkit"]
  external make: (~flipKey: string, ~children: React.element) => React.element =
    "Flipper";
};

module Flipped = {
  type flippedProps;

  [@react.component] [@bs.module "react-flip-toolkit"]
  external make:
    (
      ~flipId: string,
      ~scale: bool=?,
      ~translate: bool=?,
      ~opacity: bool=?,
      ~onAppear: (Dom.element, int) => unit=?,
      ~onExit: (Dom.element, int, unit => unit) => unit=?,
      ~children: flippedProps => React.element
    ) =>
    React.element =
    "Flipped";

  module Inverse = {
    [@react.component] [@bs.module "react-flip-toolkit"]
    external make:
      (~inverseFlipId: string, ~children: flippedProps => React.element) =>
      React.element =
      "Flipped";
  };
};

module FlippedView = {
  [@react.component] [@bs.module "./FlippedView"]
  external make:
    (~flippedProps: Flipped.flippedProps, ~children: React.element) =>
    React.element =
    "default";
};
