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
    })
  );

module CustomNetworkItem = {
  [@react.component]
  let make = (~network: Network.network, ~writeNetwork, ~settings) => {
    <>
      <View style=styles##spaceBetweenRow>
        <RadioItem
          label={network.name}
          value={`Custom(network.name)}
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
        />
        <View style=styles##row>
          <CustomNetworkEditButton network />
          <CustomNetworkDeleteButton network />
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
