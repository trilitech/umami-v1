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

module ViewWithFlippedProps = {
  [@react.component] [@bs.module "./ViewWithFlippedProps"]
  external make:
    (
      ~flippedProps: Flipped.flippedProps,
      ~zIndex: int=?,
      ~children: React.element
    ) =>
    React.element =
    "default";
};

module FlippedView = {
  [@react.component]
  let make =
      (
        ~flipId,
        ~scale=?,
        ~translate=?,
        ~opacity=?,
        ~onAppear=?,
        ~onExit=?,
        ~zIndex=?,
        ~children,
      ) => {
    <Flipped flipId ?scale ?translate ?opacity ?onAppear ?onExit>
      {flippedProps => {
         <ViewWithFlippedProps flippedProps ?zIndex>
           children
         </ViewWithFlippedProps>;
       }}
    </Flipped>;
  };

  module Inverse = {
    [@react.component]
    let make = (~inverseFlipId: string, ~children: React.element) => {
      <Flipped.Inverse inverseFlipId>
        {flippedProps => {
           <ViewWithFlippedProps flippedProps>
             children
           </ViewWithFlippedProps>;
         }}
      </Flipped.Inverse>;
    };
  };
};
