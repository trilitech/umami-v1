open ReactNative;
open Common;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~width=500.->dp,
          ~position=`absolute,
          ~bottom=20.->dp,
          ~left=150.->dp,
          ~zIndex=2,
          (),
        ),
    })
  );

module Item = {
  [@react.component]
  let make = (~indice, ~handleDelete, ~log) => {
    let fadeAnim = React.useRef(Animated.Value.create(0.)).current;

    React.useEffect1(
      () => {
        ReactUtils.startFade(fadeAnim, 1., 800., None);
        None;
      },
      [|fadeAnim|],
    );

    <Animated.View
      style=Style.(style(~opacity=fadeAnim->Animated.StyleProp.float, ()))>
      <LogItem indice log handleDelete showTimestamp=false />
    </Animated.View>;
  };
};

[@react.component]
let make = (~opacity, ~logs, ~handleDelete, ~firsts) =>
  <Animated.View
    style=Style.([|styles##container, style(~opacity, ())|]->array)>
    {logs
     ->Lib.List.firsts(firsts)
     ->Belt.List.toArray
     ->Belt.Array.mapWithIndex((i, log) =>
         <Item key={i->string_of_int} indice=i log handleDelete />
       )
     ->React.array}
  </Animated.View>;
