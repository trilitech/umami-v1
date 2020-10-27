open ReactNative;

module IconButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~width=28.->dp,
            ~height=28.->dp,
            ~marginRight=4.->dp,
            ~marginBottom=(-3.)->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            (),
          ),
        "actions": style(~alignSelf=`flexEnd, ()),
      })
    );

  [@react.component]
  let make = (~icon, ~onPress=?) => {
    <TouchableOpacity ?onPress>
      <View style=styles##button>
        <Icon name=icon size=16. color=Theme.colorDarkMediumEmphasis />
      </View>
    </TouchableOpacity>;
  };
};

module ButtonQr = {
  [@react.component]
  let make = (~account, ~balanceRequest) => {
    let modal = React.useRef(Js.Nullable.null);

    let (visibleModal, setVisibleModal) = React.useState(_ => false);
    let openAction = () => setVisibleModal(_ => true);
    let closeAction = () => setVisibleModal(_ => false);

    let onPress = _ => {
      openAction();
    };

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <IconButton icon=`qr onPress />
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <ReceiveView account balanceRequest onPressCancel />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "inner":
        style(
          ~width=355.->dp,
          ~marginLeft=14.->dp,
          ~alignSelf=`stretch,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "actionButtons": style(~alignSelf=`flexEnd, ~flexDirection=`row, ()),
    })
  );

[@react.component]
let make = (~account: Account.t) => {
  let balanceRequest = BalanceApiRequest.useBalance(account.address);

  <RowItem height=94.>
    {({hovered}: Pressable.interactionState) => {
       <>
         <View style=styles##inner>
           <Typography.Subtitle1>
             account.alias->React.string
           </Typography.Subtitle1>
           <Typography.Subtitle3>
             {switch (balanceRequest) {
              | Done(Ok(balance)) => balance->React.string
              | Done(Error(error)) => error->React.string
              | NotAsked
              | Loading =>
                <ActivityIndicator
                  animating=true
                  size={ActivityIndicator_Size.exact(17.)}
                  color=Colors.highIcon
                />
              }}
           </Typography.Subtitle3>
           <Typography.Subtitle4>
             "Address"->React.string
           </Typography.Subtitle4>
           <Typography.Body3> account.address->React.string </Typography.Body3>
         </View>
         <View
           style=Style.(
             array([|
               styles##actionButtons,
               style(~display=hovered ? `flex : `none, ()),
             |])
           )>
           <IconButton icon=`copy />
           <ButtonQr account balanceRequest />
         </View>
       </>;
     }}
  </RowItem>;
};
