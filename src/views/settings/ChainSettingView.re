/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses
  type state = {network: [ | `Mainnet | `Granadanet | `Custom(string)]}
];
module ChainForm = ReForm.Make(StateLenses);

module AddCustomNetworkButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginLeft=(-6.)->dp,
            ~marginBottom=10.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = () => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <View style=styles##button>
        <ButtonAction
          onPress
          text=I18n.Btn.add_custom_network
          icon=Icons.Add.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <NetworkFormView action=Create closeAction />
      </ModalAction>
    </>;
  };
};

module CustomNetworkEditButton = {
  [@react.component]
  let make = (~network: Network.network) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <IconButton
        tooltip=(
          "custom_network_edit" ++ network.name,
          I18n.Tooltip.custom_network_edit,
        )
        icon=Icons.Edit.build
        onPress
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <NetworkFormView action={Edit(network)} closeAction />
      </ModalAction>
    </>;
  };
};

module NetworkInfoButton = {
  [@react.component]
  let make = (~network: Network.network) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <IconButton
        tooltip=(
          "network_info" ++ network.name,
          I18n.Tooltip.see_network_info,
        )
        icon=Icons.Show.build
        onPress
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <NetworkInfoView network closeAction />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flexDirection=`row, ~alignItems=`center, ()),
      "column": style(~flex=1., ~flexDirection=`column, ()),
      "nameStatus": style(~display=`flex, ~flexDirection=`row, ()),
      "activityIndicator": style(~marginLeft=10.->dp, ()),
      "spaceBetweenRow":
        style(
          ~display=`flex,
          ~flex=1.,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "selfEnd": style(~alignSelf=`flexEnd, ()),
      "block": style(~flex=1., ~flexDirection=`column, ()),
      "leftcolumntitles": style(~justifyContent=`spaceBetween, ()),
      "chainSeparation": style(~marginTop=30.->dp, ()),
      "button": style(~height=34.->dp, ()),
      "actionMenu": style(~marginRight=24.->dp, ~flexDirection=`row, ()),
      "button": style(~marginRight=4.->dp, ()),
      "notice": style(~marginBottom=24.->dp, ~borderRadius=4., ()),
    })
  );

module CustomNetworkItem = {
  [@react.component]
  let make =
      (
        ~network: Network.network,
        ~writeNetwork,
        ~configFile: ConfigFile.t,
        ~networkStatus,
      ) => {
    let theme = ThemeContext.useTheme();
    let writeConf = ConfigContext.useWrite();

    let addToast = LogsContext.useToast();

    let deleteCustomNetwork = (networkToDelete: Network.network) => {
      writeConf(c =>
        {
          ...c,
          customNetworks:
            List.keepMap(c.customNetworks, n =>
              n === networkToDelete ? None : Some(n)
            ),
          network: {
            c.network == Some(`Custom(networkToDelete.name))
              ? None : c.network;
          },
        }
      );
    };

    let (
      tagBorderColor: option(string),
      tagTextColor: option(Typography.colorStyle),
    ) = {
      switch (network.chain) {
      | chain when chain == `Mainnet => (
          Some(theme.colors.iconPrimary),
          Some(`primary),
        )
      | _ => (Some(theme.colors.iconMediumEmphasis), Some(`mediumEmphasis))
      };
    };

    let onPressConfirmDelete = () => {
      deleteCustomNetwork(network);
      addToast(Logs.info(~origin=Logs.Account, I18n.custom_network_deleted));
      Promise.ok();
    };
    <>
      <View style=styles##spaceBetweenRow>
        <View style=styles##nameStatus>
          <RadioItem
            label={network.name}
            value={`Custom(network.name)}
            setValue=writeNetwork
            currentValue={
              configFile.network
              ->Option.getWithDefault(ConfigContext.defaultNetwork)
            }
            tag={network.chain->Network.getDisplayedName}
            tagTextColor
            tagBorderColor
          />
          {networkStatus == Some(Network.Pending)
             ? <ActivityIndicator
                 style=styles##activityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(20.)}
                 color={theme.colors.iconHighEmphasis}
               />
             : React.null}
        </View>
        <View style=styles##row>
          <CustomNetworkEditButton network />
          <DeleteButton.IconButton
            tooltip=(
              "custom_network_delete" ++ network.name,
              I18n.Tooltip.custom_network_delete,
            )
            modalTitle=I18n.Title.delete_custom_network
            onPressConfirmDelete
            request=ApiRequest.NotAsked
          />
        </View>
      </View>
    </>;
  };
};

module NetworkItem = {
  [@react.component]
  let make =
      (
        ~chain,
        ~network: Network.network,
        ~networkStatus,
        ~writeNetwork,
        ~currentConfig: ConfigFile.network,
      ) => {
    let theme = ThemeContext.useTheme();

    <>
      <View style=styles##spaceBetweenRow>
        <View style=styles##nameStatus>
          <RadioItem
            label={chain->Network.getDisplayedName}
            value=chain
            setValue=writeNetwork
            currentValue=currentConfig
          />
          {networkStatus == Some(Network.Pending)
             ? <ActivityIndicator
                 style=styles##activityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(20.)}
                 color={theme.colors.iconHighEmphasis}
               />
             : React.null}
        </View>
        <View style=styles##row> <NetworkInfoButton network /> </View>
      </View>
    </>;
  };
};

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let configFile = ConfigContext.useFile();

  let networkStatus = ConfigContext.useNetworkStatus().current;
  let offline = ConfigContext.useNetworkOffline();

  let customNetworks = configFile.customNetworks;

  let (last, setLast) =
    React.useState(_ =>
      configFile.network->Option.getWithDefault(ConfigContext.defaultNetwork)
    );

  let writeNetwork = f => {
    let network =
      configFile.network->Option.getWithDefault(ConfigContext.defaultNetwork);
    let network = f(network);
    setLast(_ => network);
    let network =
      network == ConfigContext.defaultNetwork ? None : Some(network);

    writeConf(c => {...c, network});
  };

  <Block
    title=I18n.Settings.chain_title actionButton={<AddCustomNetworkButton />}>
    <View style=styles##column>
      {<Header.Notice style=styles##notice text=I18n.Expl.network_disconnect>
         <Header.Notice.Button
           text=I18n.Btn.goto_doc
           onPress={_ =>
             System.openExternal(
               "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/wikis/doc/Custom%20Network%20Creation",
             )
           }
         />
       </Header.Notice>
       ->ReactUtils.onlyWhen(offline)}
      <View accessibilityRole=`form style=styles##row>
        <ColumnLeft style=styles##leftcolumntitles>
          <NetworkItem
            chain=`Mainnet
            networkStatus={last == `Mainnet ? Some(networkStatus) : None}
            network=Network.mainnet
            currentConfig=last
            writeNetwork
          />
          <NetworkItem
            chain=`Hangzhounet
            networkStatus={last == `Hangzhounet ? Some(networkStatus) : None}
            network=Network.hangzhounet
            currentConfig=last
            writeNetwork
          />
          {switch (customNetworks) {
           | [] => React.null
           | customNetworks =>
             customNetworks
             ->List.reverse
             ->List.toArray
             ->Array.map(network =>
                 <CustomNetworkItem
                   networkStatus={
                     last == `Custom(network.name)
                       ? Some(networkStatus) : None
                   }
                   key={network.name}
                   network
                   writeNetwork
                   configFile
                 />
               )
             ->React.array
           }}
        </ColumnLeft>
      </View>
    </View>
  </Block>;
};
