open ReactNative;

module type ICON = {
  let makeProps:
    (
      ~size: float,
      ~color: string=?,
      ~style: ReactNative.Style.t=?,
      ~key: string=?,
      unit
    ) =>
    {
      .
      "size": float,
      "color": option(string),
      "style": option(ReactNative.Style.t),
    };

  let make:
    {
      .
      "size": float,
      "color": option(string),
      "style": option(ReactNative.Style.t),
    } =>
    React.element;
};

module type SVG = {
  let makeProps:
    (
      ~width: ReactNative.Style.size=?,
      ~height: ReactNative.Style.size=?,
      ~fill: string=?,
      ~stroke: string=?,
      ~key: string=?,
      unit
    ) =>
    {
      .
      "fill": option(string),
      "height": option(ReactNative.Style.size),
      "stroke": option(string),
      "width": option(ReactNative.Style.size),
    };

  let make:
    {
      .
      "fill": option(string),
      "height": option(ReactNative.Style.size),
      "stroke": option(string),
      "width": option(ReactNative.Style.size),
    } =>
    React.element;
};

type builder =
  (~color: string=?, ~style: ReactNative.Style.t=?, ~size: float) =>
  React.element;

module Make = (Svg: SVG) => {
  module I = {
    [@react.component]
    let make =
      React.memo(
        (
          ~size: float,
          ~color: option(string)=?,
          ~style: option(ReactNative.Style.t)=?,
        ) => {
        let width = size->Style.dp;
        let height = width;
        let fill = color;
        /* let stroke = color; */
        <View ?style> <Svg width height ?fill /> </View>;
      });
  };

  let build: builder =
    (~color=?, ~style=?, ~size) => {
      let st = style;
      <I style=?st ?color size />;
    };

  include I;
};

module Home = Make(SVGIconHome);
module Send = Make(SVGIconSend);
module History = Make(SVGIconHistory);
module Receive = Make(SVGIconReceive);
module Account = Make(SVGIconAccount);
module AddressBook = Make(SVGIconAddressBook);
module Qr = Make(SVGIconQr);
module Copy = Make(SVGIconCopy);
module Close = Make(SVGIconClose);
module Add = Make(SVGIconAdd);
module ChevronDown = Make(SVGIconChevronDown);
module CheckboxUnselected = Make(SVGIconCheckboxUnselected);
module CheckboxSelected = Make(SVGIconCheckboxSelected);
module Delete = Make(SVGIconDelete);
module Edit = Make(SVGIconEdit);
module ArrowDown = Make(SVGIconArrowDown);
module More = Make(SVGIconMore);
module Delegate = Make(SVGIconDelegate);
