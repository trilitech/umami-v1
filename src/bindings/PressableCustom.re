open ReactNative;
include NativeElement;

type interactionState = {
  hovered: bool,
  focused: bool,
  pressed: bool,
};

[@react.component] [@bs.module "react-native"]
external make:
  (
    ~ref: ref=?,
    ~children: interactionState => React.element=?,
    ~delayLongPress: int=?,
    ~disabled: bool=?,
    ~hitSlop: View.edgeInsets=?,
    ~pressRetentionOffset: View.edgeInsets=?,
    ~onLayout: Event.layoutEvent => unit=?,
    ~onLongPress: Event.pressEvent => unit=?,
    ~onPress: Event.pressEvent => unit=?,
    ~onPressIn: Event.pressEvent => unit=?,
    ~onPressOut: Event.pressEvent => unit=?,
    ~style: Style.t=?,
    ~testID: string=?,
    ~testOnly_pressed: bool=?,
    ~accessibilityRole: Accessibility.role=?,
    ~href: string=?
  ) =>
  React.element =
  "Pressable";

[@bs.send]
external measureInWindow:
  (element, (~x: float, ~y: float, ~width: float, ~height: float) => unit) =>
  unit =
  "measureInWindow";
