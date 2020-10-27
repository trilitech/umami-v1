open ReactNative;

type iconName = [ | `home | `accounts | `history | `receive | `send];

[@react.component]
let make =
  React.memo(
    (
      ~name: iconName,
      ~size: float,
      ~color: option(string)=?,
      ~style: option(ReactNative.Style.t)=?,
    ) => {
    let width = size->Style.dp;
    let height = width;
    let fill = color;
    //let stroke = color;
    <View ?style>
      {switch (name) {
       | `home => <SVGIconHome width height ?fill />
       | `accounts => <SVGIconAccount width height ?fill />
       | `history => <SVGIconHistory width height ?fill />
       | `receive => <SVGIconReceive width height ?fill />
       | `send => <SVGIconSend width height ?fill />
       }}
    </View>;
  });
