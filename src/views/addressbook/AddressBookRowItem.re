open ReactNative;

let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=24.->dp, ()));
let styles =
  Style.(
    StyleSheet.create({
      "cellAlias":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=140.->dp, ()),
        |]),
      "cellAddress":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=360.->dp, ~flexGrow=1., ()),
        |]),
      "inner":
        style(~flexDirection=`row, ~width=600.->dp, ~marginLeft=22.->dp, ()),
      "actionButtons": style(~flexDirection=`row, ()),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t) => {
    <RowItem height=46.>
      {({hovered}: Pressable.interactionState) => {
         <>
           <View style=styles##inner>
             <View style=styles##cellAlias>
               <Typography.Body1>
                 account.alias->React.string
               </Typography.Body1>
             </View>
             <View style=styles##cellAddress>
               <Typography.Body1>
                 account.address->React.string
               </Typography.Body1>
             </View>
           </View>
           <View
             style=Style.(
               array([|
                 styles##actionButtons,
                 style(~display=hovered ? `flex : `none, ()),
               |])
             )>
             <IconButton icon=`copy />
             <QrButton account />
           </View>
         </>;
       }}
    </RowItem>
  });
