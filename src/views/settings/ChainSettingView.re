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
  type state = {network: [ | `Mainnet | `Florencenet | `Custom(string)]}
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
          text=I18n.btn#add_custom_network
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
          I18n.tooltip#custom_network_edit,
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

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flexDirection=`row, ~alignItems=`center, ()),
      "column": style(~flex=1., ~flexDirection=`column, ()),
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
      // "over": style(~position=`absolute, ()),
    })
  );

module CustomNetworkItem = {
  [@react.component]
  let make = (~network: Network.network, ~writeNetwork, ~settings) => {
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
      | chain when chain == Network.mainnetChain => (
          Some(theme.colors.iconPrimary),
          Some(`primary),
        )
      | _ => (Some(theme.colors.iconMediumEmphasis), Some(`mediumEmphasis))
      };
    };

    let onPressConfirmDelete = () => {
      Future.make(_ => deleteCustomNetwork(network))
      ->ApiRequest.logOk(addToast, Logs.Account, _ =>
          I18n.t#custom_network_deleted
        )
      ->ignore;
    };
    <>
      <View style=styles##spaceBetweenRow>
        <RadioItem
          label={network.name}
          value={`Custom(network.name)}
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
          tag={network.chain->Network.getChainName}
          tagTextColor
          tagBorderColor
        />
        <View style=styles##row>
          <CustomNetworkEditButton network />
          <DeleteButton.IconButton
            tooltip=(
              "custom_network_delete" ++ network.name,
              I18n.tooltip#custom_network_delete,
            )
            modalTitle=I18n.title#delete_custom_network
            onPressConfirmDelete
            request=ApiRequest.NotAsked
          />
        </View>
      </View>
    </>;
  };
};

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = SdkContext.useSettings();

  let customNetworks = ConfigContext.useContent().customNetworks;

  let writeNetwork = f => {
    let network = f(settings->AppSettings.network);
    let network =
      network == ConfigFile.Default.network ? None : Some(network);

    writeConf(c => {...c, network});
  };

  <Block title=I18n.settings#chain_title>
    <View style=styles##column>
      <View style=styles##selfEnd> <AddCustomNetworkButton /> </View>
      <View accessibilityRole=`form style=styles##row>
        <ColumnLeft style=styles##leftcolumntitles>
          <RadioItem
            label=I18n.t#mainnet
            value=`Mainnet
            setValue=writeNetwork
            currentValue={settings->AppSettings.network}
          />
          <RadioItem
            label=I18n.t#florencenet
            value=`Florencenet
            setValue=writeNetwork
            currentValue={settings->AppSettings.network}
          />
          {switch (customNetworks) {
           | [] => React.null
           | customNetworks =>
             customNetworks
             ->List.reverse
             ->List.toArray
             ->Array.map(network =>
                 <CustomNetworkItem
                   key={network.name}
                   network
                   writeNetwork
                   settings
                 />
               )
             ->React.array
           }}
        </ColumnLeft>
      </View>
    </View>
  </Block>;
};
