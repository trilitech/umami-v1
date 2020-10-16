open ReactNative;

[@bs.module "./SwitchNative"] [@react.component]
external make:
  (
    ~ref: NativeElement.ref=?,
    ~style: Style.t=?,
    ~thumbStyle: Style.t=?,
    ~value: bool=?,
    ~onValueChange: bool => unit=?,
    ~disabled: bool=?,
    ~thumbColor: Color.t=?,
    ~trackColor: Switch.trackColor=?
  ) =>
  React.element =
  "default";
