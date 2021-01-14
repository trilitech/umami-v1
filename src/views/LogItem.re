open ReactNative;
let styles =
  Style.(
    StyleSheet.create({
      "itemContent":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingLeft=20.->dp,
          ~paddingVertical=10.->dp,
          ~width=100.->pct,
          (),
        ),
      "actionButtons":
        style(~flexDirection=`row, ~flexShrink=0., ~marginLeft=auto, ()),
      "reqelt": style(~flexShrink=0., ~marginRight=5.->dp, ()),
      "itemError": style(~borderColor=Colors.error, ()),
      "itemInfo": style(~borderColor=Colors.valid, ()),
      "kindIcon": style(~marginRight=10.->dp, ()),
      "container": style(~borderRadius=3., ~marginTop=10.->dp, ()),
      "item":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~borderRadius=3.,
          ~borderLeftWidth=5.,
          ~flexWrap=`nowrap,
          ~width=100.->pct,
          ~alignSelf=`center,
          ~justifyContent=`flexStart,
          (),
        ),
      "button": style(~marginRight=4.->dp, ()),
    })
  );

module DeleteButton = {
  [@react.component]
  let make = (~isPrimary=?, ~style=?, ~indice, ~handleDelete) => {
    let onPress = _ => {
      handleDelete(indice);
    };
    <IconButton ?isPrimary ?style icon=Icons.Close.build onPress />;
  };
};

[@react.component]
let make =
    (~indice, ~log: Logs.t, ~addToast, ~showTimestamp=true, ~handleDelete) => {
  let theme = ThemeContext.useTheme();

  let kindStyle =
    switch (log.kind) {
    | Error => styles##itemError
    | Info => styles##itemInfo
    };

  let icon =
    switch (log.kind) {
    | Error => <Icons.CloseOutline size=16. color=Colors.error />
    | Info => <Icons.CheckOutline size=16. color=Colors.valid />
    };

  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.logBackground, ()),
      |])
    )>
    <Hoverable
      style=Style.(array([|styles##item, kindStyle|]))
      hoveredStyle={Style.style(
        ~backgroundColor=theme.colors.primaryStateHovered,
        (),
      )}>
      {_ => {
         <View style=styles##itemContent>
           <View style=styles##kindIcon> icon </View>
           {<Typography.Body2
              style=Style.(
                array([|
                  styles##reqelt,
                  style(~color=theme.colors.primaryTextMediumEmphasis, ()),
                |])
              )
              fontSize=12.
              fontWeightStyle=`regular
              numberOfLines=1>
              "["->React.string
              Js.Date.(log.timestamp->fromFloat->toLocaleString)->React.string
              "]  -"->React.string
            </Typography.Body2>
            ->ReactUtils.onlyWhen(showTimestamp)}
           <Typography.Body1
             style={Style.style(
               ~color=theme.colors.primaryTextMediumEmphasis,
               (),
             )}
             fontWeightStyle=`bold
             ellipsizeMode=`tail
             numberOfLines=1>
             log.msg->React.string
           </Typography.Body1>
           <View style=styles##actionButtons>
             {<ClipboardButton
                isPrimary=true
                data={log.msg}
                copied=I18n.log#log_content
                addToast
                style=styles##button
              />
              ->ReactUtils.onlyWhen(log.kind == Error)}
             <DeleteButton
               isPrimary=true
               indice
               handleDelete
               style=styles##button
             />
           </View>
         </View>;
       }}
    </Hoverable>
  </View>;
};
