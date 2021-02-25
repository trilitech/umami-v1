module Provider = {
  [@react.component] [@bs.module "@gorhom/portal"]
  external make: (~children: React.element) => React.element =
    "PortalProvider";
};

[@react.component] [@bs.module "@gorhom/portal"]
external make: (~children: React.element) => React.element = "Portal";
